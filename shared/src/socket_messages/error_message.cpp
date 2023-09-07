#include "socket_messages/error_message.h"

namespace pine::socket_messages
{
	error_message::error_message()
	{
		header.type = message_type::ERROR_MESSAGE;
		header.body_size = get_body_size();
	}

	error_message::error_message(error_code const& error_code)
		: m_error_code(error_code)
	{
		header.type = message_type::ERROR_MESSAGE;
		header.body_size = get_body_size();
	}

	bool error_message::parse_body(std::vector<uint8_t> const& buffer)
	{
		if (buffer.size() != get_body_size())
			return false;

		std::memcpy(std::bit_cast<uint8_t*>(&m_error_code), &buffer[0], sizeof(m_error_code));

		return true;
	}

	std::vector<uint8_t> error_message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size + get_body_size(), 0);
		std::vector<uint8_t> header_buffer = header.serialize();

		std::memcpy(&buffer[0], &header_buffer[0], message_header::size);
		std::memcpy(&buffer[message_header::size], std::bit_cast<uint8_t*>(&m_error_code), sizeof(m_error_code));

		return buffer;
	}

	uint64_t error_message::get_body_size() const
	{
		return sizeof(m_error_code);
	}
}