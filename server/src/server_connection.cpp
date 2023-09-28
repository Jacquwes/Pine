#include <algorithm>
#include <bit>
#include <cstdint>
#include <ios>
#include <iostream>
#include <random>

#include <asio/ip/tcp.hpp>
#include <connection.h>
#include <coroutine.h>
#include <message.h>
#include <snowflake.h>
#include <socket_messages.h>

#include "server.h"
#include "server_connection.h"


namespace pine
{
	server_connection::server_connection(
		asio::ip::tcp::socket& client_socket,
		server& server
	)
		: server_ref{ server },
		connection{ std::move(client_socket) }
	{

		std::cout << "New client connection opened: " << std::dec << id << std::endl;
	}

	async_task server_connection::listen()
	{
		co_await switch_thread(listen_thread);

		std::scoped_lock lock(connection_mutex);

		for (auto& callback : server_ref.on_connection_attemps_callbacks)
			callback(server_ref, server_connection::shared_from_this());

		if (!(co_await establish_connection()))
		{
			close();
			co_return;
		}

		std::cout << "  Client successfully connected: " << std::dec << id << std::endl;
		is_connected = true;

		for (auto& callback : server_ref.on_connection_callbacks)
			callback(server_ref, shared_from_this());

		while (is_connected)
		{
			auto&& message = co_await receive_message();

			if (message->header.type == socket_messages::message_type::INVALID_MESSAGE)
			{
				close();
				co_return;
			}

			server_ref.handle_message(shared_from_this(), message);
		}
	}

	async_operation<bool> server_connection::establish_connection()
	{
		if (!(co_await validate_connection()))
		{
			std::cout << "  Client failed validation: " << std::dec << id << std::endl;
			co_return false;
		}

		std::cout << "  Client passed validation: " << std::dec << id << std::endl;

		if (!(co_await check_version()))
		{
			std::cout << "  Client failed version check: " << std::dec << id << std::endl;
			co_return false;
		}

		std::cout << "  Client passed version check: " << std::dec << id << std::endl;

		if (!(co_await identify()))
		{
			std::cout << "  Client failed identify: " << std::dec << id << std::endl;
			co_return false;
		}

		std::cout << "  Client successfully identified in as \"" << user_data.username
			<< "\": " << std::dec << id << std::endl;

		co_return true;
	}

	async_operation<bool> server_connection::validate_connection()
	{
		std::random_device device;
		std::default_random_engine engine(device());
		std::uniform_int_distribution<uint64_t> distribution(0, -1);
		uint64_t key = distribution(engine);

		std::vector<uint8_t> keyBuffer(
			std::bit_cast<uint8_t*>(&key),
			std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t)
		);

		std::cout << "  Sending key " << std::hex << key << " to client " << std::dec
			<< id << "\n";
		std::cout << "  Client should answer " << std::hex << (key ^ 0xF007CAFEC0C0CACA)
			<< std::endl;
		co_await send_raw_message(keyBuffer);

		auto&& response = co_await receive_raw_message(sizeof(uint64_t));
		if (response.size() != sizeof(uint64_t))
			co_return false;

		key ^= 0xF007CAFEC0C0CACA;
		keyBuffer.assign(
			std::bit_cast<uint8_t*>(&key),
			std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t)
		);

		if (std::ranges::equal(keyBuffer, response))
			co_return true;
		co_return false;
	}

	async_operation<bool> server_connection::check_version()
	{
		co_await send_message(std::make_shared<socket_messages::hello_message>());

		auto&& hello = co_await receive_message();
		if (hello->header.type != socket_messages::message_type::HELLO_MESSAGE)
			co_return false;

		auto version =
			std::dynamic_pointer_cast<socket_messages::hello_message>(hello)->version;
		if (version != current_version)
			co_return false;

		co_return true;
	}

	async_operation<bool> server_connection::identify()
	{
		auto&& message = co_await receive_message();

		if (message->header.type != socket_messages::message_type::IDENTIFY_MESSAGE)
			co_return false;

		auto&& identifyMessage =
			std::dynamic_pointer_cast<socket_messages::identify_message>(message);

		user_data.username = identifyMessage->username;

		co_return true;
	}

	async_task server_connection::send_ack(snowflake id)
	{
		auto message = std::make_shared<socket_messages::acknowledge_message>(id);
		co_await send_message(message);
	}

	async_task server_connection::close()
	{
		// Return to the server thread if we're not already on it.
		if (std::this_thread::get_id() == listen_thread.get_id())
			co_await switch_thread(listen_thread);

		std::scoped_lock lock(connection_mutex);

		std::cout << "  Closing connection: " << std::dec << id << std::endl;

		this->socket.close();

		server_ref.disconnect_client(id);

		std::cout << "  Connection closed: " << std::dec << id << std::endl;

		co_return;
	}
}