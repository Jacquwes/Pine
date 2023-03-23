#pragma once

#include <deque>
#include <thread>
#include <memory>
#include <mutex>
#include <string_view>

#include "Connection.h"
#include "Coroutine.h"
#include "Snowflake.h"
#include "SocketMessages.h"

#pragma comment(lib, "ws2_32.lib")



class Server
{
public:
	void Run(std::string_view const& port = "80");
	void Stop();

	AsyncTask DisconnectClient(Snowflake clientId);
	AsyncTask MessageClient(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const& message) const;
	AsyncTask OnConnect(std::shared_ptr<Connection> const& client) const;
	AsyncTask OnMessage(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const&);

private:
#endif
	std::condition_variable m_cv;
	std::mutex m_disconnectMutex;

	bool InitSocket(std::string_view const& port);

	std::deque<std::shared_ptr<Connection>> m_clients;

	bool m_stop = true;
	SOCKET m_socket = INVALID_SOCKET;
	std::jthread m_thread;
};
