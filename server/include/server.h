#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <message.h>

#include "server_connection.h"

namespace pine
{
	/// @brief A synchronous server that accepts connections from clients.
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

		/// @brief Disconnect a client.
		/// @param client_id Id of the client to disconnect.	
		void disconnect_client(uint64_t const& client_id);

		/// @brief Send a message to a client.
		/// @param client Client to send the message to.
		/// @param message Message to send.
		void message_client(std::shared_ptr<server_connection> const& client, std::shared_ptr<socket_messages::message> const& message) const;


	private:
		/// @brief Accept clients.
		/// This function waits for clients to connect and creates a server connection for each client.
		void accept_clients();

		/// @brief Function called when a client sends a message to the server.
		/// @return 
		void handle_message(std::shared_ptr<server_connection> const& client, std::shared_ptr<socket_messages::message> const& message);

		std::mutex delete_clients_mutex;
		std::mutex mutate_clients_mutex;

		std::unordered_map<uint64_t, std::shared_ptr<server_connection>> clients;


		bool is_listening = false;
		std::jthread acceptor_thread;
		std::jthread delete_clients_thread;

		asio::io_context& io_context;
		asio::ip::tcp::acceptor acceptor;
		asio::error_code error_code;

	public:
		/// @brief Call this function to add a callback that will be executed when a client sends 
		/// a valid message to the server.
		/// @return A reference to this server.
		server& on_message(std::function<void(
			server&,
			std::shared_ptr<server_connection> const&,
			std::shared_ptr<socket_messages::message> const&)> const& callback
		);

		/// @brief Call this function to add a callback that will be executed when a new client attemps
		/// to connect to the server.
		/// @return A reference to this server.
		server& on_connection_attempt(std::function<void(
			server&,
			std::shared_ptr<server_connection> const&)> const& callback
		);

		/// @brief Call this function to add a callback that will be executed when a client fails
		/// to connect to the server.
		/// @return A reference to this server.
		server& on_connection_failed(std::function<void(
			server&,
			std::shared_ptr<server_connection> const&)> const& callback
		);

		/// @brief Call this function to add a callback that will be executed when a client successfully
		/// connects to the server.
		/// @return A reference to this server.
		server& on_connection(std::function<void(
			server&,
			std::shared_ptr<server_connection> const&)> const& callback
		);

		/// @brief Call this function to add a callback that will be executed when the server is ready
		/// to accept connections.
		/// @return A reference to this server.
		server& on_ready(std::function<void(
			server&)> const& callback
		);

	private:
		std::vector<std::function<void(
			server&,
			std::shared_ptr<server_connection> const&,
			std::shared_ptr<socket_messages::message> const&)>
		> on_message_callbacks;

		std::vector<std::function<void(
			server&,
			std::shared_ptr<server_connection> const&)>
		> on_connection_attemps_callbacks;

		std::vector<std::function<void(
			server&,
			std::shared_ptr<server_connection> const&)>
		> on_connection_failed_callbacks;

		std::vector<std::function<void(
			server&,
			std::shared_ptr<server_connection> const&)>
		> on_connection_callbacks;

		std::vector < std::function<void(
			server&)>
		> on_ready_callbacks;
	};
}
