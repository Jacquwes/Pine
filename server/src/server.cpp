#include <cstdint>
#include <iostream>
#include <memory>
#include <string_view>
#include <string>
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

	async_task server::listen()
	{
		co_await switch_thread();

		is_listening = true;

		if (!acceptor.is_open())
			acceptor.open(asio::ip::tcp::v4(), error_code);

		if (error_code)
		{
			std::cout << "[Server] Failed to open acceptor: " << error_code.message() << std::endl;
			co_return;
		}

		co_await accept_clients();

		std::cout << "[Server] Stopped listening" << std::endl;
	}

	void server::stop()
	{
		is_listening = false;

		acceptor.cancel(error_code);

		if (error_code)
		{
			std::cout << "[Server] Failed to cancel acceptor: " << error_code.message() << std::endl;
		}

		acceptor.close(error_code);

		if (error_code)
		{
			std::cout << "[Server] Failed to close acceptor: " << error_code.message() << std::endl;
		}

		auto clients_copy = clients;
		for (auto& client : clients_copy)
		{
			disconnect_client(client.first);
		}

		std::cout << "[Server] Socket stopped" << std::endl;
	}

	async_task server::accept_clients()
	{
		std::cout << "[Server] Socket starts listening" << std::endl;

		for (auto& callback : on_ready_callbacks)
		{
			co_await callback(*this);
		}

		while (is_listening)
		{
			asio::ip::tcp::socket client_socket{ io_context };

			acceptor.accept(client_socket, error_code);

			if (error_code)
			{
				std::cout << "[Server] Failed to accept client: " << error_code.message() <<
					std::endl;
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

	async_task server::disconnect_client(uint64_t const& client_id)
	{
		std::unique_lock lock{ mutate_clients_mutex };

		auto client = clients.find(client_id);

		if (client == clients.end())
		{
			co_return;
		}

		client->second->close();

		clients.erase(client_id);

		co_return;
	}

	async_task server::message_client(
		std::shared_ptr<server_connection> const& client,
		std::shared_ptr<socket_messages::message> const& message
	) const
	{
		co_await client->send_message(message);
	}

	server& server::on_message(std::function<async_task(server&, std::shared_ptr<server_connection>const&, std::shared_ptr<socket_messages::message>const&)> const& callback)
	{
		on_message_callbacks.push_back(callback);
		return *this;
	}

	server& server::on_connection_attempt(std::function<async_task(server&, std::shared_ptr<server_connection>const&)> const& callback)
	{
		on_connection_attemps_callbacks.push_back(callback);
		return *this;
	}

	server& server::on_connection_failed(std::function<async_task(server&, std::shared_ptr<server_connection>const&)> const& callback)
	{
		on_connection_failed_callbacks.push_back(callback);
		return *this;
	}

	server& server::on_connection(std::function<async_task(server&, std::shared_ptr<server_connection>const&)> const& callback)
	{
		on_connection_callbacks.push_back(callback);
		return *this;
	}

	server& server::on_ready(std::function<async_task(server&)> const& callback)
	{
		on_ready_callbacks.push_back(callback);
		return *this;
	}
}
