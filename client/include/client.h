#pragma once

#include <cstdint>
#include <functional>
#include <thread>
#include <memory>
#include <string>
#include <vector>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <coroutine.h>
#include <message.h>
#include <user.h>

#include "client_connection.h"

namespace pine
{
	/// @brief A client that can connect to a single server.
	class client
	{
		friend class client_connection;
	public:
		/// @brief Initialize a client.
		/// @param username: The username of the client.
		client(std::string username, asio::io_context& io_context);

		/// @brief Connect to a server.
		/// @param host: Hostname or ip address of the server.
		/// @param port: TCP port of the server.
		/// @return True if the connection was successful, false otherwise.
		bool connect(std::string const& host = "localhost", uint16_t const& port = 80);

		/// @brief Disconnect from the server.
		void disconnect();

		/// @brief Send a message to the server.
		async_task message_server(std::shared_ptr<socket_messages::message> const& message) const;

		/// @brief Register a callback for when a message is received.
		/// @return A reference to the client.
		client& on_message(std::function<async_task(
			std::shared_ptr<socket_messages::message>
		)> const& on_message);

		/// @brief Get the user data of the client.
		[[nodiscard]]
		constexpr user const& get_user() const;

	private:
		std::jthread connection_thread;

		std::vector<
			std::function<async_task(
				std::shared_ptr<socket_messages::message>
			)>
		> on_message_callbacks;

		asio::io_context& io_context;
		std::unique_ptr<client_connection> connection;
		user user_data;
	};
}
