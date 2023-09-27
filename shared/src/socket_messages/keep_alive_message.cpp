#include "socket_messages/keep_alive_message.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace pine::socket_messages
{
	keep_alive_message::keep_alive_message()
	{
		header.type = message_type::KEEP_ALIVE_MESSAGE;
		header.body_size = get_body_size();
	}

	bool keep_alive_message::parse_body(std::vector<uint8_t> const& buffer)
	{
		if (buffer.size() != get_body_size())
			return false;

		return true;
	}

	std::vector<uint8_t> keep_alive_message::serialize() const
	{
		return header.serialize();
	}

	size_t keep_alive_message::get_body_size() const
	{
		return 0;
	}
}
