#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <WinSock2.h>

#include "coroutine.h"
#include "message.h"
#include "snowflake.h"
#include "user.h"

namespace pine
{
	class connection : public std::enable_shared_from_this<connection>
	{
	public:
		virtual ~connection();

		async_operation<std::shared_ptr<socket_messages::message>> receive_message();
		async_task send_message(std::shared_ptr<socket_messages::message> const& message) const;

		async_operation<std::vector<uint8_t>> receive_raw_message(uint64_t const& buffer_size) const;
		async_task send_raw_message(std::vector<uint8_t> const& buffer) const;

		snowflake id;
		SOCKET socket{ INVALID_SOCKET };
		user user_data{};
	};
}