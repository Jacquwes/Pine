#pragma once

#include "pch.h"

#include "Connection.h"
#include "Coroutine.h"
#include "Snowflake.h"
#include "SocketMessages.h"

// TODO : ping

class Server
{
public:	
	void Run(std::string_view const& port = "80");
	void Stop();

	AsyncTask DisconnectClient(Snowflake clientId);
	AsyncTask MessageClient(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const& message) const;
	/*
	
	void OnDisconnect(std::shared_ptr<Connection> client) {}
	*/
	AsyncTask OnConnect(std::shared_ptr<Connection> const& client) const;
	void OnMessage(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const& message) {}
	
private:
	bool InitSocket(std::string_view const& port);
		
	std::deque<std::shared_ptr<Connection>> m_clients;

	bool m_stop = false;
	SOCKET m_socket = INVALID_SOCKET;
	std::jthread m_thread;
};
