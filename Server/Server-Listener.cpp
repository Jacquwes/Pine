module Server.Listener;

import <iostream>;
import <memory>;
import <string>;

import <WinSock2.h>;
import <ws2tcpip.h>;

import Server.Connection;
import Server.Exception;
import Socket.Snowflake;

namespace Server
{
	void Listener::Run(std::string_view const& port)
	{
		if (!InitSocket(port))
		{
			throw Exception{ "Failed to initialize socket: " + WSAGetLastError() };
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
				throw Exception{ "Failed to accept client: " + WSAGetLastError() };
			}

			auto client = std::make_shared<Connection>(clientSocket, *this);
			client->Listen();
			m_clients.push_back(std::move(client));
		}

		std::cout << "Socket stops listening" << std::endl;
	}



	void Listener::Stop()
	{
		m_stop = true;
		closesocket(m_socket);
		WSACleanup();
	}



	AsyncTask Listener::DisconnectClient(Socket::Snowflake clientId)
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
			throw Exception{ "Trying to disconnect inexistent client." };

		if ((*client)->GetThread().joinable())
			(*client)->GetThread().join();
		m_clients.erase(client);
	}



	AsyncTask Listener::MessageClient(std::shared_ptr<Connection> const& client, std::shared_ptr<Socket::Messages::Message> const& message) const
	{
		std::vector<uint8_t> buffer;

		switch (message->header.type)
		{
		case Socket::Messages::MessageType::Hello:
		{
			buffer = std::dynamic_pointer_cast<Socket::Messages::Hello>(message)->Serialize();
			break;
		}
		default:
			throw Exception{ "Trying to send unknown message type." };
			break;
		}

		co_await client->SendRawMessage(buffer);
	}



	AsyncTask Listener::OnConnect(std::shared_ptr<Connection> const& client) const
	{
		auto hello = std::make_shared<Socket::Messages::Hello>();
		co_await MessageClient(client, std::static_pointer_cast<Socket::Messages::Message>(hello));
	}



	bool Listener::InitSocket(std::string_view const& port)
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
}