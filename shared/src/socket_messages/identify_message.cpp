#include "socket_messages/identify_message.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	identify_message::identify_message()
	{
		header.type = message_type::IDENTIFY_MESSAGE;
		header.body_size = get_body_size();
	}

	identify_message::identify_message(const std::string& name)
		: username(name)
	{
		header.type = message_type::IDENTIFY_MESSAGE;
		header.body_size = get_body_size();
	}

	bool identify_message::parse_body(std::vector<uint8_t> const& buffer)
	{
		if (buffer.size() < sizeof(uint8_t) + username_min_length
			|| buffer.size() > sizeof(uint8_t) + username_max_length)
			return false;

		username = std::string(buffer.begin() + sizeof(uint8_t), buffer.end());

		header.body_size = get_body_size();

		return true;
	}

	std::vector<uint8_t> identify_message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size + get_body_size(), 0);
		std::vector<uint8_t>::iterator it = buffer.begin();

		it = std::copy_n(header.serialize().begin(), message_header::size, it);
		uint8_t username_length = username.size();
		it = std::copy_n(reinterpret_cast<uint8_t const*>(&username_length), sizeof(uint8_t), it);
		it = std::copy_n(reinterpret_cast<uint8_t const*>(username.c_str()), username_length, it);

		return buffer;
	}

	uint64_t identify_message::get_body_size() const
	{
		return sizeof(uint8_t) + username.size();
	}
}