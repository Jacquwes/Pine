#include <ios>
#include <iostream>
#include <memory>

#include <connection.h>
#include <coroutine.h>
#include <message.h>
#include <socket_messages.h>
#include <snowflake.h>

#include "server.h"
#include "server_connection.h"

namespace pine
{
	async_task server::on_message(std::shared_ptr<connection> client, std::shared_ptr<socket_messages::message> message)
	{
		std::cout << "  Message received from client: " << std::dec << client->id << std::endl;

		switch (message->header.type)
		{
			using enum socket_messages::message_type;

		case ACKNOWLEDGE_MESSAGE:
		case ERROR_MESSAGE:
		case RECEIVE_CHAT_MESSAGE:
			std::cout << "  Message type not expected from client: " << std::dec << static_cast<int>(message->header.type) << std::endl;
			disconnect_client(client->id);
			break;

		case HELLO_MESSAGE:
		case IDENTIFY_MESSAGE:
			std::cout << "  Message type not expected from client now: " << std::dec << static_cast<int>(message->header.type) << std::endl;
			break;

		case SEND_CHAT_MESSAGE:
		{
			std::cout << "  Received chat message from client: " << std::dec << client->id << std::endl;
			auto receive_chat = std::make_shared<socket_messages::receive_chat_message>();
			receive_chat->author_username = client->user_data.username;
			receive_chat->message_content = std::dynamic_pointer_cast<socket_messages::send_chat_message>(message)->message_content;

			for (auto const& i : clients)
			{
				if (client->id == i.first)
					continue;
				co_await std::dynamic_pointer_cast<server_connection>(client)->send_message(receive_chat);
			}

			break;
		}

		default:
			std::cout << "  Unknown message type: " << std::dec << static_cast<int>(message->header.type) << std::endl;
			disconnect_client(client->id);
			break;
		}

		co_return;
	}
}