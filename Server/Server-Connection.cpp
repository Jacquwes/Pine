module Server.Connection;

import <iostream>;
import <random>;
import <WinSock2.h>;
import <ws2tcpip.h>;

import Server.Coroutines;
import Server.Listener;
import Server.Exception;
import Socket.Messages;

namespace Server
{
	Connection::Connection(SOCKET socket, Listener& listener)
		: m_listener{ listener }
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
			m_listener.DisconnectClient(m_id);
			co_return;
		}
		std::cout << "  Client passed validation: " << std::dec << m_id << std::endl;

		co_await m_listener.OnConnect(shared_from_this());

		while (true)
		{
			std::vector<uint8_t>&& receivedMessage = co_await ReceiveRawMessage(sizeof(Socket::Messages::MessageHeader));
			if (receivedMessage.size() != sizeof(Socket::Messages::MessageHeader::type) + sizeof(Socket::Messages::MessageHeader::size))
			{
				m_listener.DisconnectClient(m_id);
				co_return;
			}

			Socket::Messages::MessageHeader header{ receivedMessage };
			if (header.type == Socket::Messages::MessageType::Invalid)
			{
				m_listener.DisconnectClient(m_id);
				co_return;
			}

			std::vector<uint8_t>&& body = co_await ReceiveRawMessage(header.size);

			Socket::Messages::Message message;
			message.header = header;

			m_listener.OnMessage(shared_from_this(), std::make_shared<Socket::Messages::Message>(message));
		}
	}



	AsyncOperation<std::vector<uint8_t>> Connection::ReceiveRawMessage(size_t const& bufferSize) const
	{
		std::vector<uint8_t> buffer(bufferSize, 0);

		if (!bufferSize)
			co_return buffer;

		int n = recv(m_socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(bufferSize), 0);
		if (n == SOCKET_ERROR)
		{
			throw Exception{ "Failed to receive message: " + WSAGetLastError() };
		}

		buffer.resize(n);
		co_return buffer;
	}



	AsyncTask Connection::SendRawMessage(std::vector<uint8_t> const& buffer) const
	{
		int n = send(m_socket, std::bit_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0);
		if (n == SOCKET_ERROR)
		{
			throw Exception{ "Failed to send message: " + WSAGetLastError() };
		}

		co_return;
	}



	AsyncOperation<bool> Connection::ValidateConnection() const
	{
		std::random_device device;
		std::default_random_engine engine(device());
		std::uniform_int_distribution<uint64_t> distribution(0, -1);
		uint64_t key = distribution(engine);

		std::vector<uint8_t> keyBuffer(std::bit_cast<uint8_t*>(&key), std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t));

		std::cout << "  Sending key " << std::hex << key << " to client " << std::dec << m_id << std::endl;
		co_await SendRawMessage(keyBuffer);

		auto&& response = co_await ReceiveRawMessage(sizeof(uint64_t));
		if (response.size() != sizeof(uint64_t))
			co_return false;

		key ^= 0xF007CAFEC0C0CA7E;
		keyBuffer.assign(std::bit_cast<uint8_t*>(&key), std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t));

		if (std::equal(keyBuffer.begin(), keyBuffer.end(), response.begin()))
			co_return true;
		co_return false;
	}
}