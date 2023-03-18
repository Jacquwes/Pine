#include <iostream>
#include <memory>
#include <random>
#include <string_view>
#include <vector>

#include "Connection.h"
#include "Coroutine.h"
#include "Server.h"
#include "ServerException.h"
#include "SocketMessages.h"

Connection::Connection(SOCKET socket, Server& server)
	: m_server{ server }
	, m_socket{ socket }
{
	std::cout << "New client connected: " << std::dec << m_id << std::endl;
}



Connection::~Connection()
{
	std::cout << "Client disconnected: " << std::dec << m_id << std::endl;
	
	if (m_socket != INVALID_SOCKET)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
	}
}



AsyncTask Connection::Listen()
{
	co_await SwitchThread(m_thread);

	if (bool validated = !(co_await ValidateConnection()))
	{
		std::cout << "  Client failed validation: " << std::dec << m_id << std::endl;
		m_server.DisconnectClient(m_id);
		co_return;
	}
	std::cout << "  Client passed validation: " << std::dec << m_id << std::endl;

	co_await m_server.OnConnect(shared_from_this());

	if (!(co_await CheckVersion()))
	{
		std::cout << "  Client failed version check: " << std::dec << m_id << std::endl;
			m_server.DisconnectClient(m_id);
			co_return;
		}
		
	std::cout << "  Client passed version check: " << std::dec << m_id << std::endl;

		{
			m_server.DisconnectClient(m_id);
			co_return;
		}

		std::vector<uint8_t>&& body = co_await ReceiveRawMessage(header.size);
		
		SocketMessages::Message message;
		message.header = header;
		
		m_server.OnMessage(shared_from_this(), std::make_shared<SocketMessages::Message>(message));
	}
}


AsyncOperation<std::vector<uint8_t>> Connection::ReceiveRawMessage(uint64_t const& bufferSize) const
{
	std::vector<uint8_t> buffer(bufferSize, 0);
	
	if (!bufferSize)
		co_return buffer;
	
	int n = recv(m_socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(bufferSize), 0);
	if (n == SOCKET_ERROR)
	{
		throw ServerException{ "Failed to receive message: " + WSAGetLastError() };
	}

	buffer.resize(n);
	co_return buffer;
}



AsyncTask Connection::SendRawMessage(std::vector<uint8_t> const& buffer) const
{
	int n = send(m_socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0);
	if (n == SOCKET_ERROR)
	{
		throw ServerException{ "Failed to send message: " + WSAGetLastError() };
	}
	
	co_return;
}



AsyncOperation<bool> Connection::CheckVersion() const
{
	std::shared_ptr<SocketMessages::Message> hello = co_await ReceiveMessage();
	if (hello->header.messageType != SocketMessages::MessageType::Hello)
		co_return false;

	auto version = std::dynamic_pointer_cast<SocketMessages::Hello>(hello)->version;
	if (version != CURRENT_VERSION)
		co_return false;

	co_return true;
}

AsyncOperation<bool> Connection::ValidateConnection() const
{
	std::random_device device;
	std::default_random_engine engine(device());
	std::uniform_int_distribution<uint64_t> distribution(0, -1);
	uint64_t key = distribution(engine);

	std::vector<uint8_t> keyBuffer(std::bit_cast<uint8_t*>(&key), std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t));

	std::cout << "  Sending key " << std::hex << key << " to client " << std::dec << m_id << std::endl;
	std::cout << "  Client should answer " << std::hex << (key ^ 0xF007CAFEC0C0CA7E) << std::endl;
	co_await SendRawMessage(keyBuffer);
	
	auto&& response = co_await ReceiveRawMessage(sizeof(uint64_t));
	if (response.size() != sizeof(uint64_t))
		co_return false;

	key ^= 0xF007CAFEC0C0CA7E;
	keyBuffer.assign(std::bit_cast<uint8_t*>(&key), std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t));
	
	if (std::ranges::equal(keyBuffer, response))
		co_return true;
	co_return false;
}