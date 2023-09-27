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
	/// @brief A server that accepts connections from clients.
	class server
	{
		friend class server_connection;

	public:
		/// @brief Construct a server with the given asio context and port.
		server(asio::io_context& io_context, uint16_t const& port = 80);

		/// @brief Start listening for connections.
		void listen();

		/// @brief Stop listening for connections.
		void stop();

		/// @brief Send a message to a client.
		/// @param client Client to send the message to.
		/// @param message Message to send.
		/// @return An asynchronous task completed when the message has been sent.
		async_task message_client(std::shared_ptr<connection> const& client, std::shared_ptr<socket_messages::message> const& message) const;

		/// @brief Function called when a client sends a message to the server.
		/// @param client Client that sent the message.
		/// @param message Message sent by the client.
		/// @return An asynchronous task completed when the message has been processed.
		async_task on_message(std::shared_ptr<connection> client, std::shared_ptr<socket_messages::message> message);

	private:
		/// @brief Accept clients.
		/// This function waits for clients to connect and creates a server connection for each client.
		/// @return An asynchronous task completed when the server has stopped listening.
		async_task accept_clients();

		/// @brief Delete disconnected clients.
		/// This function deletes disconnected clients from the server.
		/// @return An asynchronous task completed when the server has stopped listening.
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