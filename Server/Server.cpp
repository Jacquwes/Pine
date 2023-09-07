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
		throw ServerException{ "Failed to initialize socket: " + std::to_string(WSAGetLastError()) };
	}

	std::cout << "Pine - version " << std::hex << CurrentVersion << "\nServer Listening" << std::endl;

	m_stop = false;

	DeleteClients();

	while (!m_stop)
	{
		SOCKET clientSocket = accept(m_socket, nullptr, nullptr);

		if (m_stop)
			break;

		if (clientSocket == INVALID_SOCKET)
		{
			closesocket(m_socket);
			WSACleanup();
			throw ServerException{ "Failed to accept client: " + std::to_string(WSAGetLastError()) };
		}

		auto client = std::make_shared<ServerConnection>(clientSocket, *this);
		client->Listen();

		std::unique_lock lock{ m_mutateClientsMutex };
		m_clients.insert({ client->GetId(), std::move(client) });
	}

	std::cout << "Socket stops listening" << std::endl;
}



void Server::Stop()
{
	m_stop = true;

	for (auto const& client : m_clients)
	{
		DisconnectClient(client.first);
	}

	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
	WSACleanup();
}



AsyncTask Server::DeleteClients()
{
	co_await SwitchThread(m_deleteClientsThread);

	while (!m_stop)
	{
		std::unique_lock lock { m_deleteClientsMutex };
		m_deleteClients.wait(lock);
		m_clientsToDelete.clear();
	}
}



AsyncTask Server::DisconnectClient(uint64_t clientId)
{
	std::unique_lock lock{ m_mutateClientsMutex };

	auto client = m_clients.find(clientId);

	if (client == m_clients.end())
	{
		co_return;
	}

	closesocket(client->second->GetSocket());
	shutdown(client->second->GetSocket(), SD_BOTH);

	m_clientsToDelete.push_back(std::move(client->second));

	m_clients.erase(clientId);

	m_deleteClients.notify_one();
	
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
