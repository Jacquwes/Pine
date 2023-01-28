#include "Connection.h"
#include "Coroutine.h"
#include "Server.h"
#include "ServerException.h"

Connection::Connection(SOCKET socket, Server& server)
	: m_server{ server }
	, m_socket{ socket }
{
	std::cout << "New client connected" << std::endl;
}

Connection::~Connection()
{
	std::cout << "Client disconnected" << std::endl;
	
	if (m_socket != INVALID_SOCKET)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
	}
}

AsyncTask Connection::Listen()
{
	co_await SwitchThread(m_thread);


	bool validated = co_await ValidateConnection();
	if (!validated)
	{
		m_server.DisconnectClient(m_id);
		co_return;
	}
}

AsyncOperation<std::vector<uint8_t>> Connection::ReceiveRawMessage(size_t const& bufferSize) const
{
	std::vector<uint8_t> buffer(bufferSize, 0);
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

AsyncOperation<bool> Connection::ValidateConnection() const
{
	std::cout << "Validate connection:" << std::endl;
	std::random_device device;
	std::default_random_engine engine(device());
	std::uniform_int_distribution<uint64_t> distribution(0, -1);
	uint64_t key = distribution(engine);

	std::vector<uint8_t> keyBuffer(std::bit_cast<uint8_t*>(&key), std::bit_cast<uint8_t*>(&key) + sizeof(uint64_t));

	std::cout << "  Sending key " << std::hex << key << std::endl;
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