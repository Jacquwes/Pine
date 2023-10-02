#include <cstdint>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>

#include "connection.h"
#include "message.h"
#include "snowflake.h"
#include "socket_messages/ack_message.h"
#include "socket_messages/hello_message.h"
#include "socket_messages/identify_message.h"
#include "socket_messages/keep_alive_message.h"
#include "socket_messages/receive_chat_message.h"
#include "socket_messages/send_chat_message.h"

namespace pine
{
	connection::connection(asio::ip::tcp::socket socket) :
		socket(std::move(socket))
	{
		std::cout << "[Connection] New connection: " << id << std::endl;
	}

	connection::~connection()
	{}

	std::vector<uint8_t> connection::receive_raw_message(uint64_t const& buffer_size)
	{
		std::vector<uint8_t> buffer(buffer_size, 0);

		if (!buffer_size)
			return buffer;

		asio::error_code ec;
		size_t n = socket.receive(asio::buffer(buffer), {}, ec);

		if (ec && ec != asio::error::connection_reset && ec != asio::error::connection_aborted)
		{
			std::cout << "[Connection] Failed to receive message: " << std::dec << ec.value() << " -> " << ec.message() << std::endl;
			return buffer;
		}

		buffer.resize(n);

		return buffer;
	}

	void connection::send_raw_message(std::vector<uint8_t> const& buffer)
	{
		if (buffer.empty())
			return;

		asio::error_code ec;
		socket.send(asio::buffer(buffer), {}, ec);

		if (ec && ec != asio::error::connection_reset && ec != asio::error::connection_aborted)
		{
			std::cout << "[Connection] Failed to send message: " << std::dec << ec.value() << " -> " << ec.message() << std::endl;
			return;
		}
	}

	std::shared_ptr<socket_messages::message> connection::receive_message()
	{
		auto message = std::make_shared<socket_messages::message>();

		std::vector<uint8_t> received_message = receive_raw_message(socket_messages::message_header::size);
		if (received_message.size() != socket_messages::message_header::size)
			return message;

		socket_messages::message_header header{ received_message };
		if (header.type == socket_messages::message_type::INVALID_MESSAGE)
			return message;

		std::vector<uint8_t> body = receive_raw_message(header.body_size);

		switch (header.type)
		{
			using namespace socket_messages;

		case ACKNOWLEDGE_MESSAGE:
			message = std::make_shared<acknowledge_message>();

			if (!std::dynamic_pointer_cast<acknowledge_message>(message)->parse_body(body))
				return message;
			break;

		case HELLO_MESSAGE:
			message = std::make_shared<hello_message>();

			if (!std::dynamic_pointer_cast<hello_message>(message)->parse_body(body))
				return message;
			break;

		case IDENTIFY_MESSAGE:
			message = std::make_shared<identify_message>();

			if (!std::dynamic_pointer_cast<identify_message>(message)->parse_body(body))
				return message;
			break;

		case KEEP_ALIVE_MESSAGE:
			message = std::make_shared<keep_alive_message>();

			if (!std::dynamic_pointer_cast<keep_alive_message>(message)->parse_body(body))
				return message;
			break;

		case SEND_CHAT_MESSAGE:
			message = std::make_shared<send_chat_message>();

			if (!std::dynamic_pointer_cast<send_chat_message>(message)->parse_body(body))
				return message;
			break;

		case RECEIVE_CHAT_MESSAGE:
			message = std::make_shared<receive_chat_message>();

			if (!std::dynamic_pointer_cast<receive_chat_message>(message)->parse_body(body))
				return message;
			break;

		default:
			return message;
		}

		message->header = header;

		return message;
	}

	void connection::send_message(std::shared_ptr<socket_messages::message> const& message)
	{
		std::vector<uint8_t> buffer = message->serialize();

		send_raw_message(buffer);
	}

	void connection::close()
	{
		socket.close();
	}
}