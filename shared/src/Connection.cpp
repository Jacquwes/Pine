#include <cstdint>
#include <exception>
#include <iostream>
#include <vector>

#include <asio.hpp>

#include "connection.h"
#include "coroutine.h"
#include "snowflake.h"
#include "socket_messages.h"

namespace pine
{
	connection::~connection()
	{
		std::cout << "Closing connection: " << id << std::endl;

        close();
		}



    async_operation<std::vector<uint8_t>> connection::receive_raw_message(uint64_t const& bufferSize)
	{
		std::vector<uint8_t> buffer(bufferSize, 0);

		if (!bufferSize)
			co_return buffer;

		int n = recv(socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(bufferSize), 0);
		if (n == SOCKET_ERROR)
		{
			int error = WSAGetLastError();

			if (error == WSAECONNRESET || error == WSAECONNABORTED)
			{
				co_return buffer;
			}

			throw "Failed to receive message: " + WSAGetLastError();
		}

		buffer.resize(n);
		co_return buffer;
	}


    async_task connection::send_raw_message(std::vector<uint8_t> const& buffer)

        try {
	{
		int n = send(socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0);
		if (n == SOCKET_ERROR)
		{
			throw "Failed to send message: " + WSAGetLastError();
		}

		co_return;
	}



	async_operation<std::shared_ptr<socket_messages::message>> connection::receive_message()
	{
		auto message = std::make_shared<socket_messages::message>();

		std::vector<uint8_t> received_message = co_await receive_raw_message(socket_messages::message_header::size);
		if (received_message.size() != socket_messages::message_header::size)
			co_return message;

		socket_messages::message_header header{ received_message };
		if (header.type == socket_messages::message_type::INVALID_MESSAGE)
			co_return message;

		std::vector<uint8_t> body = co_await receive_raw_message(header.body_size);


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



    async_task connection::send_message(std::shared_ptr<socket_messages::message> const& message)
	{
		std::vector<uint8_t> buffer = message->serialize();

		co_await send_raw_message(buffer);
	}

    void connection::close()
    {
        socket.close();
    }
}