#include "socket_messages/ack_message.h"

#include <bit>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	acknowledge_message::acknowledge_message()
	{
		header.type = message_type::acknowledge_message;
		header.body_size = get_body_size();
	}

	acknowledge_message::acknowledge_message(snowflake const& id_)
		: id{ id_ }
	{
		header.type = message_type::acknowledge_message;
		header.body_size = get_body_size();
	}

	bool acknowledge_message::parse_body(std::vector<uint8_t> const& buffer)
	{
		if (buffer.size() != get_body_size())
			return false;

		uint64_t new_id{};
		std::memcpy(std::bit_cast<void*>(&new_id), std::bit_cast<void*>(buffer.data()), sizeof(new_id));
		id = snowflake(new_id);

		return true;
	}

	std::vector<uint8_t> acknowledge_message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size + get_body_size(), 0);
		std::vector<uint8_t> header_buffer = header.serialize();

		std::memcpy(&buffer[0], &header_buffer[0], message_header::size);
		auto id_ = static_cast<uint64_t>(id);
		std::memcpy(&buffer[message_header::size], &id_, sizeof(id_));

		return buffer;
	}

	uint64_t acknowledge_message::get_body_size() const
	{
		return sizeof(snowflake::value);
	}
}
