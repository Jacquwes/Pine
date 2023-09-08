#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include <WinSock2.h>
#include <message.h>

#include "connection.h"
#include "coroutine.h"
#include "server_connection.h"

#pragma comment(lib, "ws2_32.lib")


namespace pine
{
	class server
	{
	public:
		void run(std::string_view const& port = "80");
		void stop();

		async_task disconnect_client(uint64_t client_id);
		async_task message_client(std::shared_ptr<connection> const& client, std::shared_ptr<socket_messages::message> const& message) const;
		async_task on_connect(std::shared_ptr<connection> const& client) const;
		async_task on_message(std::shared_ptr<connection> client, std::shared_ptr<socket_messages::message> message);

		bool init_socket(std::string_view const& port);

	private:
		async_task delete_clients();

		std::condition_variable delete_clients_cv;
		std::mutex delete_clients_mutex;
		std::mutex mutate_clients_mutex;

		std::unordered_map<uint64_t, std::shared_ptr<server_connection>> clients;
		std::vector<std::shared_ptr<server_connection>> clients_to_delete;

		bool stop_listening = true;
		SOCKET socket = INVALID_SOCKET;
		std::jthread thread;
		std::jthread delete_clients_thread;
	};
}