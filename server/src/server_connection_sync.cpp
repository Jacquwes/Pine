#include <algorithm>
#include <bit>
#include <cstdint>
#include <ios>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include <asio/ip/tcp.hpp>
#include <connection.h>
#include <message.h>
#include <snowflake.h>
#include <socket_messages/ack_message.h>
#include <socket_messages/hello_message.h>
#include <socket_messages/identify_message.h>

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

		std::cout << "[Server] New client connection opened: " << std::dec << id << std::endl;
	}

	void server_connection::listen()
	{
		std::scoped_lock lock(connection_mutex);

		for (auto& callback : server_ref.on_connection_attemps_callbacks)
			callback(server_ref, server_connection::shared_from_this());

		if (!(establish_connection()))
			close();

		std::cout << "[Server]   Client successfully connected: " << std::dec << id << std::endl;
		is_connected = true;

		for (auto& callback : server_ref.on_connection_callbacks)
			callback(server_ref, shared_from_this());

		while (is_connected)
		{
			auto&& message = receive_message();

			if (message->header.type == socket_messages::message_type::INVALID_MESSAGE)
				close();

			server_ref.handle_message(shared_from_this(), message);
		}
	}

	bool server_connection::establish_connection()
	{
		if (!(validate_connection()))
		{
			std::cout << "[Server]   Client failed validation: " << std::dec << id << std::endl;
			return false;
		}

		std::cout << "[Server]   Client passed validation: " << std::dec << id << std::endl;

		if (!(check_version()))
		{
			std::cout << "[Server]   Client failed version check: " << std::dec << id << std::endl;
			return false;
		}

		std::cout << "[Server]   Client passed version check: " << std::dec << id << std::endl;

		if (!(identify()))
		{
			std::cout << "[Server]   Client failed identify: " << std::dec << id << std::endl;
			return false;
		}

		std::cout << "[Server]   Client successfully identified in as \"" << user_data.username
			<< "\": " << std::dec << id << std::endl;

		return true;
	}

	bool server_connection::validate_connection()
	{
		std::random_device device;
		std::default_random_engine engine(device());
		std::uniform_int_distribution<uint64_t> distribution(0, -1);
		uint64_t key = distribution(engine);

		std::vector<uint8_t> keyBuffer(
			std::bit_cast<uint8_t*>(&key),
			std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t)
		);

		std::cout << "[Server]   Sending key " << std::hex << key << " to client " << std::dec
			<< id << "\n";
		std::cout << "[Server]   Client should answer " << std::hex << (key ^ 0xF007CAFEC0C0CACA)
			<< std::endl;
		send_raw_message(keyBuffer);

		auto&& response = receive_raw_message(sizeof(uint64_t));
		if (response.size() != sizeof(uint64_t))
			return false;

		key ^= 0xF007CAFEC0C0CACA;
		keyBuffer.assign(
			std::bit_cast<uint8_t*>(&key),
			std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t)
		);

		if (std::ranges::equal(keyBuffer, response))
			return true;
		return false;
	}

	bool server_connection::check_version()
	{
		send_message(std::make_shared<socket_messages::hello_message>());

		auto&& hello = receive_message();
		if (hello->header.type != socket_messages::message_type::HELLO_MESSAGE)
			return false;

		auto version =
			std::dynamic_pointer_cast<socket_messages::hello_message>(hello)->version;
		if (version != current_version)
			return false;

		return true;
	}

	bool server_connection::identify()
	{
		auto&& message = receive_message();

		if (message->header.type != socket_messages::message_type::IDENTIFY_MESSAGE)
			return false;

		auto&& identifyMessage =
			std::dynamic_pointer_cast<socket_messages::identify_message>(message);

		user_data.username = identifyMessage->username;

		return true;
	}

	void server_connection::send_ack(snowflake id)
	{
		auto message = std::make_shared<socket_messages::acknowledge_message>(id);
		send_message(message);
	}

	void server_connection::close()
	{
		this->socket.close();

		std::scoped_lock lock(connection_mutex);

		std::cout << "[Server]   Closing connection: " << std::dec << id << std::endl;

		server_ref.disconnect_client(id);

		std::cout << "[Server]   Connection closed: " << std::dec << id << std::endl;
	}
}