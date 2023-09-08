#include "socket_messages/error_message.h"

#include <cstdint>
#include <vector>

#include "message.h"

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

		m_error_code = static_cast<error_code>(buffer[0]);

		return true;
	}

	std::vector<uint8_t> error_message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size + get_body_size(), 0);
		std::vector<uint8_t>::iterator it = buffer.begin();

		it = std::copy_n(header.serialize().begin(), message_header::size, it);
		it = std::copy_n(reinterpret_cast<uint8_t const *>(&m_error_code), sizeof(m_error_code), it);

		return buffer;
	}

	uint64_t error_message::get_body_size() const
	{
		return sizeof(m_error_code);
	}
}