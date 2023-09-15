#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <asio.hpp>

#include "coroutine.h"
#include "message.h"
#include "snowflake.h"
#include "user.h"

namespace pine
{
	class connection : public std::enable_shared_from_this<connection>
	{
	public:
		connection(asio::ip::tcp::socket& socket);
		virtual ~connection();

		async_operation<std::shared_ptr<socket_messages::message>> receive_message();
		async_task send_message(std::shared_ptr<socket_messages::message> const& message);

		async_operation<std::vector<uint8_t>> receive_raw_message(uint64_t const& buffer_size);
		async_task send_raw_message(std::vector<uint8_t> const& buffer);

		void close();

		snowflake id{};
		asio::ip::tcp::socket socket;
		user user_data{};
	};
}