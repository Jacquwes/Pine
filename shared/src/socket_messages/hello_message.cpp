#include "socket_messages/hello_message.h"

#include <bit>
#include <cstdint>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	hello_message::hello_message()
	{
		header.type = message_type::HELLO_MESSAGE;
		header.body_size = get_body_size();
	}

	bool hello_message::parse_body(std::vector<uint8_t> const& buffer)
	{
		if (buffer.size() != get_body_size())
			return false;

		std::memcpy(std::bit_cast<void*>(&m_version), std::bit_cast<void*>(buffer.data()), sizeof(m_version));

		return true;
	}

	std::vector<uint8_t> hello_message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size + get_body_size(), 0);
		std::vector<uint8_t> headerBuffer = header.serialize();

		std::memcpy(&buffer[0], &headerBuffer[0], message_header::size);
		std::memcpy(&buffer[message_header::size], &m_version, sizeof(m_version));

		return buffer;
	}

	uint64_t hello_message::get_body_size() const
	{
		return sizeof(m_version);
	}
}