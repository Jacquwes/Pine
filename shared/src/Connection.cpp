#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

#include <asio.hpp>

#include "connection.h"
#include "coroutine.h"
#include "snowflake.h"
#include "socket_messages.h"

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
		auto flags = asio::socket_base::message_peek;
		size_t n = socket.receive(asio::buffer(buffer), flags, ec);

		if (ec && ec != asio::error::connection_reset && ec != asio::error::connection_aborted)
		{
			std::cout << "[Connection] Failed to receive message: " << std::dec << ec.value() << " -> " << ec.message() << std::endl;
			return buffer;
		}

		buffer.resize(n);

		return buffer;
	}

	async_operation<std::vector<uint8_t>> connection::receive_raw_message_async(uint64_t const& buffer_size)
	{
		std::vector<uint8_t> buffer(buffer_size, 0);

		if (!buffer_size)
			co_return buffer;

		asio::error_code ec;
		auto flags = asio::socket_base::message_peek;
		size_t n = socket.receive(asio::buffer(buffer), flags, ec);

		if (ec && ec != asio::error::connection_reset && ec != asio::error::connection_aborted)
		{
			std::cout << "[Connection] Failed to receive message: " << std::dec << ec.value() << " -> " << ec.message() << std::endl;
			co_return buffer;
		}

		buffer.resize(n);

		co_return buffer;
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

	async_task connection::send_raw_message_async(std::vector<uint8_t> const& buffer)
	{
		if (buffer.empty())
			co_return;

		asio::error_code ec;
		socket.send(asio::buffer(buffer), {}, ec);

		if (ec && ec != asio::error::connection_reset && ec != asio::error::connection_aborted)
		{
			std::cout << "[Connection] Failed to send message: " << std::dec << ec.value() << " -> " << ec.message() << std::endl;
			co_return;
		}

		co_return;
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

	async_operation<std::shared_ptr<socket_messages::message>> connection::receive_message_async()
	{
		auto message = std::make_shared<socket_messages::message>();

		std::vector<uint8_t> received_message = co_await receive_raw_message_async(socket_messages::message_header::size);
		if (received_message.size() != socket_messages::message_header::size)
			co_return message;

		socket_messages::message_header header{ received_message };
		if (header.type == socket_messages::message_type::INVALID_MESSAGE)
			co_return message;

		std::vector<uint8_t> body = co_await receive_raw_message_async(header.body_size);

		switch (header.type)
		{
			using namespace socket_messages;

		case ACKNOWLEDGE_MESSAGE:
			message = std::make_shared<acknowledge_message>();

			if (!std::dynamic_pointer_cast<acknowledge_message>(message)->parse_body(body))
				co_return message;
			break;

		case HELLO_MESSAGE:
			message = std::make_shared<hello_message>();

			if (!std::dynamic_pointer_cast<hello_message>(message)->parse_body(body))
				co_return message;
			break;

		case IDENTIFY_MESSAGE:
			message = std::make_shared<identify_message>();

			if (!std::dynamic_pointer_cast<identify_message>(message)->parse_body(body))
				co_return message;
			break;

		case KEEP_ALIVE_MESSAGE:
			message = std::make_shared<keep_alive_message>();

			if (!std::dynamic_pointer_cast<keep_alive_message>(message)->parse_body(body))
				co_return message;
			break;

		case SEND_CHAT_MESSAGE:
			message = std::make_shared<send_chat_message>();

			if (!std::dynamic_pointer_cast<send_chat_message>(message)->parse_body(body))
				co_return message;
			break;

		case RECEIVE_CHAT_MESSAGE:
			message = std::make_shared<receive_chat_message>();

			if (!std::dynamic_pointer_cast<receive_chat_message>(message)->parse_body(body))
				co_return message;
			break;

		default:
			co_return message;
		}


		message->header = header;

		/* todo: if (dynamic_cast<ServerConnection*>(this) != nullptr)
			co_await((ServerConnection*)this)->SendAck(header.messageId);
		*/

		co_return message;
	}

	void connection::send_message(std::shared_ptr<socket_messages::message> const& message)
	{
		std::vector<uint8_t> buffer = message->serialize();

		send_raw_message(buffer);
	}

	async_task connection::send_message_async(std::shared_ptr<socket_messages::message> const& message)
	{
		std::vector<uint8_t> buffer = message->serialize();

		co_await send_raw_message_async(buffer);
	}

	void connection::close()
	{
		socket.close();
	}
}