#include "socket_messages/identify_message.h"

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
		std::vector<uint8_t> header_buffer = header.serialize();

		std::memcpy(&buffer[0], &header_buffer[0], message_header::size);
		buffer[message_header::size] = username.length();
		std::memcpy(&buffer[message_header::size + sizeof(uint8_t)], &username[0], username.length());

		return buffer;
	}

	size_t identify_message::get_body_size() const
	{
		return sizeof(uint8_t) + username.length();
	}

	constexpr bool identify_message::check_username(std::string_view const& name)
	{
		if (username.length() < username_min_length || username.length() > username_max_length)
			return false;

		return true;
	}
}