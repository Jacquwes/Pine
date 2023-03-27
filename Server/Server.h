#pragma once

#include <condition_variable>
#include <unordered_map>
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

	AsyncTask DisconnectClient(uint64_t clientId);
	AsyncTask MessageClient(std::shared_ptr<Connection> const& client, std::shared_ptr<SocketMessages::Message> const& message) const;
	AsyncTask OnConnect(std::shared_ptr<Connection> const& client) const;
	AsyncTask OnMessage(std::shared_ptr<Connection> client, std::shared_ptr<SocketMessages::Message> message);

private:
	AsyncTask DeleteClients();

	std::condition_variable m_deleteCliens;
	std::mutex m_deleteClientsMutex;
	std::mutex m_mutateClientsMutex;

	bool InitSocket(std::string_view const& port);

	std::unordered_map<uint64_t, std::shared_ptr<ServerConnection>> m_clients;
	std::vector<std::shared_ptr<ServerConnection>> m_clientsToDelete;

	bool m_stop = true;
	SOCKET m_socket = INVALID_SOCKET;
	std::jthread m_thread;
	std::jthread m_deleteClientsThread;
};
