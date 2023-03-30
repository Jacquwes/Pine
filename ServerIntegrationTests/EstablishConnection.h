#pragma once

#include "Connection.h"

namespace ServerIntegrationTests
{
	AsyncTask EstablishConnection(ClientConnection& client)
	{
		client.Connect();
		std::vector<uint8_t> validationToken = co_await client.ReceiveRawMessage(sizeof(uint64_t));
		*std::bit_cast<uint64_t*>(validationToken.data()) ^= 0xF007CAFEC0C0CACA;
		co_await client.SendRawMessage(validationToken);
		std::shared_ptr<SocketMessages::Message> validationResponse = co_await client.ReceiveMessage();

		auto helloMessage = std::make_shared<SocketMessages::HelloMessage>();
		co_await client.SendMessage(helloMessage);
		std::shared_ptr<SocketMessages::Message> helloResponse = co_await client.ReceiveMessage();

		auto identifyMessage = std::make_shared<SocketMessages::IdentifyMessage>();
		identifyMessage->SetUsername("Username");
		co_await client.SendMessage(identifyMessage);
		std::shared_ptr<SocketMessages::Message> identifyResponse = co_await client.ReceiveMessage();
	}
}