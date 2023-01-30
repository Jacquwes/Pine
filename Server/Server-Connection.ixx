export module Server.Connection;

import <WinSock2.h>;
import <memory>;
import <vector>;
import <thread>;


import Server.Coroutines;
import Socket.Snowflake;

export namespace Server
{
	class Listener;
	
	class Connection : public std::enable_shared_from_this<Connection>
	{
	public:
		Connection(SOCKET socket, Listener& listener);
		~Connection();

		AsyncTask Listen();

		AsyncOperation<std::vector<uint8_t>> ReceiveRawMessage(size_t const& bufferSize) const;
		AsyncTask SendRawMessage(std::vector<uint8_t> const& buffer) const;

		AsyncOperation<bool> ValidateConnection() const;

		[[nodiscard]] constexpr Socket::Snowflake const& GetId() const noexcept { return m_id; }
		[[nodiscard]] constexpr std::jthread& GetThread() noexcept { return m_thread; }

	private:
		std::jthread m_thread;
		Socket::Snowflake m_id;
		Listener& m_listener;
		SOCKET m_socket;
	};
}