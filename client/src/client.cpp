#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include <coroutine.h>
#include <message.h>
#include <user.h>

#include "client.h"
#include "client_connection.h"

namespace pine
{
	client::client(std::string username, asio::io_context& io_context)
		: connection{ std::make_unique<client_connection>(*this, io_context) },
		user_data{ username },
		io_context{ io_context }
	{}

	bool client::connect(std::string const& host, uint16_t const& port)
	{
		bool result = connection->connect(host, port);

		if (!result)
			return false;

		connection_thread = std::jthread{ [this]() {
			connection->listen();
		} };

		return true;
	}

	void client::disconnect()
	{
		connection->disconnect();
	}

	async_task client::message_server(std::shared_ptr<socket_messages::message> const& message) const
	{
		co_await this->connection->send_message(message);
	}

	client& client::on_message(std::function<async_task(
		std::shared_ptr<socket_messages::message>
	)> const& on_message)
	{
		on_message_callbacks.push_back(on_message);

		return *this;
	}

	constexpr user const& client::get_user() const
	{
		return user_data;
	}
}
