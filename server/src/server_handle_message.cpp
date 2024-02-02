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
	async_task server::handle_message(std::shared_ptr<server_connection> const& client, std::shared_ptr<socket_messages::message> const& message)
	{
		std::cout << "[Server]   Message received from client: " << std::dec << client->id << std::endl;

		switch (message->header.type)
		{
			using enum socket_messages::message_type;

		case ACKNOWLEDGE_MESSAGE:
		case ERROR_MESSAGE:
		case RECEIVE_CHAT_MESSAGE:
			std::cout << "[Server]   Message type not expected from client: " << std::dec << static_cast<int>(message->header.type) << std::endl;
			disconnect_client(client->id);
			break;

		case HELLO_MESSAGE:
		case IDENTIFY_MESSAGE:
			std::cout << "[Server]   Message type not expected from client now: " << std::dec << static_cast<int>(message->header.type) << std::endl;
			break;

		case SEND_CHAT_MESSAGE:
		{
			std::cout << "[Server]   Received chat message from client: " << std::dec << client->id << std::endl;
			for (auto& callback : on_message_callbacks)
				co_await callback(*this, client, message);
			break;
		}

		default:
			std::cout << "[Server]   Unknown message type: " << std::dec << static_cast<int>(message->header.type) << std::endl;
			disconnect_client(client->id);
			break;
		}

		co_return;
	}
}