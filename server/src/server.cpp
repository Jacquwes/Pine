#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include <connection.h>
#include <message.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

#include "server_connection.h"
#include "server.h"

namespace pine
{
	void server::run(std::string_view const& port)
	{
		if (!init_socket(port))
		{
			throw "Failed to initialize socket: " + std::to_string(WSAGetLastError());
		}

		std::cout << "Pine - version " << std::hex << current_version << "\nServer Listening" << std::endl;

		stop_listening = false;

		delete_clients();

		while (!stop_listening)
		{
			SOCKET client_socket = accept(socket, nullptr, nullptr);

			if (stop_listening)
				break;

			if (client_socket == INVALID_SOCKET)
			{
				closesocket(socket);
				WSACleanup();
				throw "Failed to accept client: " + std::to_string(WSAGetLastError());
			}

			auto client = std::make_shared<server_connection>(client_socket, *this);
			client->listen();

			std::unique_lock lock{ mutate_clients_mutex };
			clients.insert({ client->id, std::move(client) });
		}

		std::cout << "Socket stops listening" << std::endl;
	}



	void server::stop()
	{
		stop_listening = true;

		for (auto const& client : clients)
		{
			disconnect_client(client.first);
		}

		shutdown(socket, SD_BOTH);
		closesocket(socket);
		WSACleanup();
	}



	async_task server::delete_clients()
	{
		co_await switch_thread(delete_clients_thread);

		while (!stop_listening)
		{
			std::unique_lock lock{ delete_clients_mutex };
			delete_clients_cv.wait(lock);
			clients_to_delete.clear();
		}
	}



	async_task server::disconnect_client(uint64_t client_id)
	{
		std::unique_lock lock{ mutate_clients_mutex };

		auto client = clients.find(client_id);

		if (client == clients.end())
		{
			co_return;
		}

		closesocket(client->second->socket);
		shutdown(client->second->socket, SD_BOTH);

		clients_to_delete.push_back(std::move(client->second));

		clients.erase(client_id);

		delete_clients_cv.notify_one();

		co_return;
	}



	async_task server::message_client(std::shared_ptr<connection> const& client, std::shared_ptr<socket_messages::message> const& message) const
	{
		co_await client->send_message(message);
	}



	async_task server::on_connect(std::shared_ptr<connection> const& client) const
	{
		std::cout << "  Client connected: " << std::dec << client->id << std::endl;

		co_return;
	}



	bool server::init_socket(std::string_view const& port)
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

		addrinfo* local_address = nullptr;
		addrinfo hints =
		{
			.ai_family = AF_INET,
			.ai_socktype = SOCK_STREAM,
			.ai_protocol = IPPROTO_TCP,
		};

		if (int i = getaddrinfo(nullptr, port.data(), &hints, &local_address); i)
		{
			WSACleanup();
			return false;
		}

		if (socket = ::socket(local_address->ai_family, local_address->ai_socktype, local_address->ai_protocol); socket == INVALID_SOCKET)
		{
			freeaddrinfo(local_address);
			WSACleanup();
			return false;
		}

		if (bind(socket, local_address->ai_addr, static_cast<int>(local_address->ai_addrlen)) == SOCKET_ERROR || listen(socket, SOMAXCONN) == SOCKET_ERROR)
		{
			freeaddrinfo(local_address);
			closesocket(socket);
			WSACleanup();
			return false;
		}

		return true;
	}
}
