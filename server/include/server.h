#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include <connection.h>
#include <coroutine.h>
#include <message.h>

#include "server_connection.h"

namespace pine
{
	class server
	{
		friend class server_connection;

	public:
		server(asio::io_context& io_context, uint16_t const& port = 80);

		void listen();
		void stop();

		async_task disconnect_client(uint64_t const& client_id);
		async_task message_client(std::shared_ptr<connection> const& client, std::shared_ptr<socket_messages::message> const& message) const;

		async_task on_message(std::shared_ptr<connection> client, std::shared_ptr<socket_messages::message> message);

	private:
		async_task accept_clients();
		async_task delete_disconnected_clients();

		std::condition_variable delete_clients_cv;
		std::mutex delete_clients_mutex;
		std::mutex mutate_clients_mutex;

		std::unordered_map<uint64_t, std::shared_ptr<server_connection>> clients;
		std::vector<std::shared_ptr<server_connection>> disconnected_clients;

		bool is_listening = false;
		std::jthread acceptor_thread;
		std::jthread delete_clients_thread;

		asio::io_context& io_context;
		asio::ip::tcp::acceptor acceptor;
		asio::error_code error_code;
	};
}