#include <iostream>
#include <vector>
#include <memory>
#include <ranges>
#include <string_view>
#include <thread>
#include <WS2tcpip.h>


#include "Server.h"
#include "ServerException.h"
#include "SocketMessages.h"

void Server::Run(std::string_view const& port)
{
	if (!InitSocket(port))
	{
		throw ServerException{ "Failed to initialize socket: " + WSAGetLastError() };
	}

	std::cout << "Pine - version " << std::hex << CurrentVersion << "\nServer Listening" << std::endl;

	m_stop = false;
	bool firstLoop = true;

	while (!m_stop)
	{
		if (firstLoop)
		{
			firstLoop = false;
			m_cv.notify_one();
		}

		SOCKET clientSocket = accept(m_socket, nullptr, nullptr);
		
		if (m_stop)
			break;

		if (clientSocket == INVALID_SOCKET)
		{
			closesocket(m_socket);
			WSACleanup();
			throw ServerException{ "Failed to accept client: " + WSAGetLastError() };
		}

		auto client = std::make_shared<Connection>(clientSocket, *this);
		client->Listen();

		std::unique_lock lock{ m_mutateClients };
		m_clients.push_back(std::move(client));
	}

	std::cout << "Socket stops listening" << std::endl;
}



void Server::Stop()
{
	m_stop = true;

	for (auto const& client : m_clients)
	{
		if (!client->IsDisconnecting())
			DisconnectClient(client->GetId());
	}
	
	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
	WSACleanup();
	m_cv.notify_one();
}



AsyncTask Server::DisconnectClient(Snowflake clientId)
{
	std::unique_lock lock{ m_mutateClients };


	auto client = std::ranges::find_if(m_clients, [clientId](auto const& potentialClient)
									   {
										   return potentialClient->GetId() == clientId;
									   });

	if (client != m_clients.end() && !(*client)->IsDisconnecting())
	{
		(*client)->SetDisconnecting(true);
		closesocket((*client)->GetSocket());
		shutdown((*client)->GetSocket(), SD_BOTH);

		if ((*client)->GetThread().joinable())
		{
			(*client)->GetThread().request_stop();
			(*client)->GetThread().join();
		}

		m_clients.erase(client);
	}

	m_cv.notify_all();

	co_return;
}



AsyncTask Server::MessageClient(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const& message) const
{
	co_await client->SendMessage(message);
}



AsyncTask Server::OnConnect(std::shared_ptr<Connection> const& client) const
{
	std::cout << "  Client connected: " << std::dec << client->GetId() << std::endl;

	co_return;
}



bool Server::InitSocket(std::string_view const& port)
{

	WSADATA data = { 0 };

	if (int i = WSAStartup(MAKEWORD(2, 2), &data); i)
	{
		return false;
	}

	if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}

	addrinfo* localAddress = nullptr;
	addrinfo hints =
	{
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = IPPROTO_TCP,
	};

	if (int i = getaddrinfo(nullptr, port.data(), &hints, &localAddress); i)
	{
		WSACleanup();
		return false;
	}

	if (m_socket = socket(localAddress->ai_family, localAddress->ai_socktype, localAddress->ai_protocol); m_socket == INVALID_SOCKET)
	{
		freeaddrinfo(localAddress);
		WSACleanup();
		return false;
	}

	if (bind(m_socket, localAddress->ai_addr, static_cast<int>(localAddress->ai_addrlen)) == SOCKET_ERROR || listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		freeaddrinfo(localAddress);
		closesocket(m_socket);
		WSACleanup();
		return false;
	}

	return true;
}
