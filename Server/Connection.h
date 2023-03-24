#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <WinSock2.h>

#include "Coroutine.h"
#include "SocketMessages.h"
#include "Snowflake.h"
#include "User.h"

#undef SendMessage

class Server;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(SOCKET socket, Server& server);
	~Connection();

	AsyncTask Listen();
	
	AsyncOperation<std::shared_ptr<SocketMessages::Message>> ReceiveMessage();
	AsyncTask SendMessage(std::shared_ptr<SocketMessages::Message> const& message) const;
	
	AsyncOperation<std::vector<uint8_t>> ReceiveRawMessage(uint64_t const& bufferSize) const;
	AsyncTask SendRawMessage(std::vector<uint8_t> const& buffer) const;

	[[nodiscard]] constexpr Snowflake const& GetId() const noexcept { return m_id; }
	[[nodiscard]] constexpr std::jthread& GetThread() noexcept { return m_thread; }
	[[nodiscard]] constexpr std::shared_ptr<User> const& GetUser() const noexcept { return m_user; }
	[[nodiscard]] constexpr SOCKET const& GetSocket() const noexcept { return m_socket; }

	[[nodiscard]] constexpr bool const& IsDisconnecting() const noexcept { return m_disconnecting; }
	constexpr void SetDisconnecting(bool const& disconnecting) noexcept { m_disconnecting = disconnecting; }

private:

	AsyncOperation<bool> EstablishConnection();
	AsyncOperation<bool> CheckVersion();
	AsyncOperation<bool> Identify();
	AsyncOperation<bool> ValidateConnection() const;

	AsyncTask SendAck() const;

	Snowflake m_id;
	Snowflake m_lastMessageId;
	Server& m_server;
	SOCKET m_socket;
	std::shared_ptr<User> m_user{ std::make_shared<User>() };
	std::jthread m_thread;
	bool m_disconnecting{ false };
};
