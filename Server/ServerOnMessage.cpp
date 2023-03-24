#include "Server.h"
#include "Connection.h"
#include "Coroutine.h"
#include "SocketMessages.h"
#include "Snowflake.h"

#include <iostream>
#include <memory>

AsyncTask Server::OnMessage(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const& message)
{
	std::cout << "  Message received from client: " << std::dec << client->GetId() << std::endl;

	switch (message->header.messageType)
	{
		using enum SocketMessages::MessageType;

	case AcknowledgeMessage:
		[[fallthroug]]
	case ErrorMessage:
		[[fallthroug]]
	case ReceiveChatMessage:
		std::cout << "  Message type not expected from client: " << std::dec << static_cast<int>(message->header.messageType) << std::endl;
		DisconnectClient(client->GetId());
		break;

	case HelloMessage:
		[[fallthroug]]
	case IdentifyMessage:
		std::cout << "  Message type not expected from client now: " << std::dec << static_cast<int>(message->header.messageType) << std::endl;
		break;

	default:
		std::cout << "  Unknown message type: " << std::dec << static_cast<int>(message->header.messageType) << std::endl;
		DisconnectClient(client->GetId());
		break;
	}

	co_return;
}
