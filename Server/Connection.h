#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include <WinSock2.h>

#include "Coroutine.h"
#include "SocketMessages.h"
#include "Snowflake.h"

#undef SendMessage

class Server;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	virtual ~Connection();
	
	AsyncOperation<std::shared_ptr<SocketMessages::Message>> ReceiveMessage();
	AsyncTask SendMessage(std::shared_ptr<SocketMessages::Message> const& message) const;
	
	AsyncOperation<std::vector<uint8_t>> ReceiveRawMessage(uint64_t const& bufferSize) const;
	AsyncTask SendRawMessage(std::vector<uint8_t> const& buffer) const;

	[[nodiscard]] constexpr Snowflake const& GetId() const noexcept { return m_id; }
	[[nodiscard]] constexpr User& GetUser() noexcept { return m_user; }
	[[nodiscard]] constexpr SOCKET const& GetSocket() const noexcept { return m_socket; }

protected:
	Snowflake m_id;
	SOCKET m_socket{ INVALID_SOCKET };
	User m_user{};
};

class ClientConnection : public Connection
{
	friend class Connection;

	public:
		ClientConnection();

		bool Connect();
};

class ServerConnection : public Connection
{
	friend class Connection;

public: 
	ServerConnection(SOCKET& socket, Server& server);

	AsyncTask Listen();

	[[nodiscard]] constexpr std::jthread& GetListenThread() noexcept { return m_listenThread; }
	AsyncTask SendAck(Snowflake id) const;

private:
	AsyncOperation<bool> EstablishConnection();
	AsyncOperation<bool> ValidateConnection();
	AsyncOperation<bool> CheckVersion();
	AsyncOperation<bool> Identify();

	Server& m_server;
	std::jthread m_listenThread;
};