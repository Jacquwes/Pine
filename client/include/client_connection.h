#pragma once

#include <cstdint>
#include <string>
#include <thread>

#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <connection.h>
#include <coroutine.h>

namespace pine
{
	class client;

	class client_connection : public connection
	{
	public:
		/// @brief Construct a client connection.
		/// @param client Reference to the owning client.
		/// @param io_context Reference to the asio context.
		client_connection(client& client, asio::io_context& io_context);

		/// @brief Connect to a server.
		/// @param host Hostname or ip address of the server.
		/// @param port TCP port of the server.
		/// @return True if the connection was successful, false otherwise.
		bool connect(std::string const& host, uint16_t const& port = 80);

		/// @brief Disconnect from the server.
		void disconnect();

		/// @brief Start listening for messages from the server.
		/// @return An asynchronous task completed when the connection has been closed.
		async_task listen();

		/// @brief Error code of the connection.
		asio::error_code ec;

	private:
		std::jthread client_thread;
		client& client_ref;
	};
}