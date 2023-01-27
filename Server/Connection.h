#pragma once

#include "pch.h"
#include "Coroutine.h"
#include "Snowflake.h"

class Server;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(SOCKET socket, Server& server);
	~Connection();

	AsyncTask Listen();
	
	AsyncOperation<std::vector<uint8_t>> ReceiveRawMessage(size_t const& bufferSize) const;
	AsyncTask SendRawMessage(std::vector<uint8_t> const& buffer) const;
	
	AsyncOperation<bool> ValidateConnection() const;

	[[nodiscard]] constexpr Snowflake const& GetId() const noexcept { return m_id; }
	[[nodiscard]] constexpr std::jthread& GetThread() noexcept { return m_thread; }
	
private:
	std::jthread m_thread;
	Snowflake m_id;
	Server& m_server;
	SOCKET m_socket;
};
