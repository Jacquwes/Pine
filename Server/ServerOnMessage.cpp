#include "Server.h"
#include "Connection.h"
#include "Coroutine.h"
#include "SocketMessages.h"
#include "Snowflake.h"

#include <iostream>
#include <memory>

AsyncTask Server::OnMessage(std::shared_ptr<Connection> client, std::shared_ptr<SocketMessages::Message> message)
{
	std::cout << "  Message received from client: " << std::dec << client->GetId() << std::endl;

	switch (message->header.messageType)
	{
		using enum SocketMessages::MessageType;

	case AcknowledgeMessage:
	case ErrorMessage:
	case ReceiveChatMessage:
		std::cout << "  Message type not expected from client: " << std::dec << static_cast<int>(message->header.messageType) << std::endl;
		DisconnectClient(client->GetId());
		break;

	case HelloMessage:
	case IdentifyMessage:
		std::cout << "  Message type not expected from client now: " << std::dec << static_cast<int>(message->header.messageType) << std::endl;
		break;

	case SendChatMessage:
	{
		std::cout << "  Received chat message from client: " << std::dec << client->GetId() << std::endl;
		auto receiveChat = std::make_shared<SocketMessages::ReceiveChatMessage>();
		receiveChat->SetAuthorUsername(client->GetUser().GetUsername());
		receiveChat->SetChatMessage(std::dynamic_pointer_cast<SocketMessages::SendChatMessage>(message)->GetChatMessage());

		co_await client->SendAck();

		for (auto const& iteratedClient : m_clients)
		{
			if (iteratedClient->GetId() == client->GetId())
				continue;
			co_await client->SendMessage(receiveChat);
		}

		break;
	}

	default:
		std::cout << "  Unknown message type: " << std::dec << static_cast<int>(message->header.messageType) << std::endl;
		DisconnectClient(client->GetId());
		break;
	}

	co_return;
}
