module;

#include <deque>
#include <memory>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>

export module Server.Listener;

import Server.Connection;
import Server.Coroutines;
import Socket.Messages;
import Socket.Snowflake;

// TODO : ping

export namespace Server
{
	class Listener
	{
	public:
		void Run(std::string_view const& port = "80");
		void Stop();

		AsyncTask DisconnectClient(Socket::Snowflake clientId);
		AsyncTask MessageClient(std::shared_ptr<Connection> const& client, std::shared_ptr<Socket::Messages::Message> const& message) const;
		/*

		void OnDisconnect(std::shared_ptr<Connection> client) {}
		*/
		AsyncTask OnConnect(std::shared_ptr<Connection> const& client) const;
		void OnMessage(std::shared_ptr<Connection> const& client, std::shared_ptr<Socket::Messages::Message> const& message) {}

	private:
		bool InitSocket(std::string_view const& port);

		std::deque<std::shared_ptr<Connection>> m_clients;

		bool m_stop = false;
		SOCKET m_socket = INVALID_SOCKET;
		std::jthread m_thread;
	};
}