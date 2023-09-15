#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <coroutine.h>

#include "server.h"
#include "server_connection.h"

namespace pine
{
	server::server(asio::io_context& context, uint16_t const& port)
		: io_context{ context },
		acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
	{}

	void server::listen()
	{
		delete_disconnected_clients();

		is_listening = true;
		acceptor.open(asio::ip::tcp::v4(), error_code);

		if (error_code)
		{
			std::cout << "Failed to open acceptor: " << error_code.message() << std::endl;
			return;
		}

		accept_clients();

		std::cout << "Socket stops listening" << std::endl;
	}

	void server::stop()
	{
		is_listening = false;

		acceptor.cancel(error_code);

		if (error_code)
		{
			std::cout << "Failed to cancel acceptor: " << error_code.message() << std::endl;
		}

		acceptor.close(error_code);

		if (error_code)
		{
			std::cout << "Failed to close acceptor: " << error_code.message() << std::endl;
		}

		for (auto const& client : clients)
		{
			disconnect_client(client.first);
		}

		std::cout << "Socket stopped listening" << std::endl;
	}

	async_task server::accept_clients()
	{
		while (is_listening)
		{
			asio::ip::tcp::socket client_socket{ io_context };

			acceptor.accept(client_socket, error_code);

			if (error_code)
			{
				std::cout << "Failed to accept client: " << error_code.message() << std::endl;
				continue;
			}

			if (!is_listening)
				break;

			auto client = std::make_shared<server_connection>(client_socket, *this);
			client->listen();

			{
				std::unique_lock lock{ mutate_clients_mutex };
				clients.insert({ client->id, std::move(client) });
			}
		}

		co_return;
	}

	async_task server::delete_disconnected_clients()
	{
		co_await switch_thread(delete_clients_thread);

		while (!is_listening)
		{
			std::unique_lock lock{ delete_clients_mutex };
			delete_clients_cv.wait(lock);
			disconnected_clients.clear();
		}
	}

	async_task server::disconnect_client(uint64_t const& client_id)
	{
		std::unique_lock lock{ mutate_clients_mutex };

		auto client = clients.find(client_id);

		if (client == clients.end())
		{
			co_return;
		}

		client->second->close();

		disconnected_clients.push_back(std::move(client->second));

		clients.erase(client_id);

		delete_clients_cv.notify_one();

		co_return;
	}

	async_task server::message_client(std::shared_ptr<connection> const& client, std::shared_ptr<socket_messages::message> const& message) const
	{
		co_await client->send_message(message);
	}
}
