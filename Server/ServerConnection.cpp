#include <iostream>
#include <memory>
#include <random>
#include <Winsock2.h>

#include "Connection.h"
#include "Server.h"



ServerConnection::ServerConnection(SOCKET& clientSocket, Server& server)
	: m_server{ server }
{
	m_socket = clientSocket;

	std::cout << "New client connection opened: " << m_id << std::endl;
}



AsyncTask ServerConnection::Listen()
{
	co_await SwitchThread(m_listenThread);

	if (!(co_await EstablishConnection()))
	{
		m_server.DisconnectClient(m_id);
		co_return;
	}

	co_await m_server.OnConnect(shared_from_this());

	while (true)
	{
		auto&& message = co_await ReceiveMessage();

		if (message->header.messageType == SocketMessages::MessageType::InvalidMessage)
		{
			m_server.DisconnectClient(m_id);
			co_return;
		}

		m_server.OnMessage(shared_from_this(), message);
	}
}



AsyncOperation<bool> ServerConnection::EstablishConnection()
{
	if (!(co_await ValidateConnection()))
	{
		std::cout << "  Client failed validation: " << m_id << std::endl;
		co_return false;
	}

	std::cout << "  Client passed validation: " << m_id << std::endl;

	if (!(co_await CheckVersion()))
	{
		std::cout << "  Client failed version check: " << m_id << std::endl;
		co_return false;
	}

	std::cout << "  Client passed version check: " << m_id << std::endl;

	if (!(co_await Identify()))
	{
		std::cout << "  Client failed identify: " << m_id << std::endl;
		co_return false;
	}

	std::cout << "  Client successfully identified in as \"" << m_user.GetUsername() << "\": " << std::dec << m_id << std::endl;

	co_return true;
}



AsyncOperation<bool> ServerConnection::ValidateConnection()
{
	std::random_device device;
	std::default_random_engine engine(device());
	std::uniform_int_distribution<uint64_t> distribution(0, -1);
	uint64_t key = distribution(engine);

	std::vector<uint8_t> keyBuffer(std::bit_cast<uint8_t*>(&key), std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t));

	std::cout << "  Sending key " << std::hex << key << " to client " << std::dec << m_id << std::endl;
	std::cout << "  Client should answer " << std::hex << (key ^ 0xF007CAFEC0C0CACA) << std::endl;
	co_await SendRawMessage(keyBuffer);

	auto&& response = co_await ReceiveRawMessage(sizeof(uint64_t));
	if (response.size() != sizeof(uint64_t))
		co_return false;

	key ^= 0xF007CAFEC0C0CACA;
	keyBuffer.assign(std::bit_cast<uint8_t*>(&key), std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t));

	if (std::ranges::equal(keyBuffer, response))
		co_return true;
	co_return false;
}



AsyncOperation<bool> ServerConnection::CheckVersion()
{
	co_await SendMessage(std::make_shared<SocketMessages::HelloMessage>());

	auto&& hello = co_await ReceiveMessage();
	if (hello->header.messageType != SocketMessages::MessageType::HelloMessage)
		co_return false;

	auto version = std::dynamic_pointer_cast<SocketMessages::HelloMessage>(hello)->GetVersion();
	if (version != CurrentVersion)
		co_return false;

	co_return true;
}



AsyncOperation<bool> ServerConnection::Identify()
{
	auto&& message = co_await ReceiveMessage();

	if (message->header.messageType != SocketMessages::MessageType::IdentifyMessage)
		co_return false;

	auto&& identifyMessage = std::dynamic_pointer_cast<SocketMessages::IdentifyMessage>(message);

	m_user.SetUsername(identifyMessage->GetUsername());

	co_return true;
}



AsyncTask ServerConnection::SendAck(Snowflake id) const
{
	auto message = std::make_shared<SocketMessages::AcknowledgeMessage>(id);
	co_await SendMessage(message);
}
