#include <iostream>
#include <vector>
#include <memory>
#include <ranges>
#include <string_view>
#include <thread>
#include <WS2tcpip.h>


#include "Server.h"
#include "ServerException.h"

void Server::Run(std::string_view const& port)
{
	if (!InitSocket(port))
	{
		throw ServerException{ "Failed to initialize socket: " + WSAGetLastError() };
	}

	std::cout << "Server Listening" << std::endl;

	m_stop = false;
	while (!m_stop)
	{
		SOCKET clientSocket = accept(m_socket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET)
		{
			closesocket(m_socket);
			WSACleanup();
			throw ServerException{ "Failed to accept client: " + WSAGetLastError() };
		}

		auto client = std::make_shared<Connection>(clientSocket, *this);
		client->Listen();
		m_clients.push_back(std::move(client));
	}

	std::cout << "Socket stops listening" << std::endl;
}



void Server::Stop()
{
	m_stop = true;
	closesocket(m_socket);
	WSACleanup();
}



AsyncTask Server::DisconnectClient(Snowflake clientId)
{
	if (m_thread.joinable())
	{
		m_thread.request_stop();
		m_thread.join();
	}

	co_await SwitchThread(m_thread); // Allow the connection thread to end

	auto client = std::ranges::find_if(std::ranges::begin(m_clients), std::ranges::end(m_clients), [clientId](auto const& potentialClient)
									   {
										   return potentialClient->GetId() == clientId;
									   });

	if (client == m_clients.end())
		throw ServerException{ "Trying to disconnect inexistent client." };

	if ((*client)->GetThread().joinable())
		(*client)->GetThread().join();
	m_clients.erase(client);
}



AsyncTask Server::MessageClient(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const& message) const
{
	std::vector<uint8_t> buffer;

	switch (message->header.type)
	{
	case SocketMessages::MessageType::Hello:
	{
		buffer = std::dynamic_pointer_cast<SocketMessages::Hello>(message)->Serialize();
		break;
	}
	default:
		throw ServerException{ "Trying to send unknown message type." };
		break;
	}

	co_await client->SendRawMessage(buffer);
}



AsyncTask Server::OnConnect(std::shared_ptr<Connection> const& client) const
{
	auto hello = std::make_shared<SocketMessages::Hello>();
	co_await MessageClient(client, std::static_pointer_cast<SocketMessages::Message>(hello));
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
