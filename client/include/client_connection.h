#pragma once

#include <cstdint>
#include <string>
#include <thread>

#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <connection.h>
#include <coroutine.h>

namespace pine
{
	class client_connection : public connection
	{
	public:
		client_connection(asio::ip::tcp::socket& socket);

		async_operation<bool> connect(std::string const& host, uint16_t const& port = 80);

		asio::error_code ec;

	private:
		std::jthread client_thread;
	};
}