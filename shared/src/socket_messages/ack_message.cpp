#include "socket_messages/ack_message.h"

#include <bit>
#include <cstdint>
#include <vector>

#include "message.h"
#include "snowflake.h"

namespace pine::socket_messages
{
	acknowledge_message::acknowledge_message()
	{
		header.type = message_type::ACKNOWLEDGE_MESSAGE;
		header.body_size = get_body_size();
	}

	acknowledge_message::acknowledge_message(snowflake const& id)
		: acknowledged_message_id{ id }
	{
		header.type = message_type::ACKNOWLEDGE_MESSAGE;
		header.body_size = get_body_size();
	}

	bool acknowledge_message::parse_body(std::vector<uint8_t> const& buffer)
	{
		if (buffer.size() != get_body_size())
			return false;

		uint64_t new_id{};
		std::copy_n(buffer.begin(), sizeof(new_id), reinterpret_cast<uint8_t*>(&new_id));
		acknowledged_message_id = snowflake(new_id);

		return true;
	}

	std::vector<uint8_t> acknowledge_message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size + get_body_size(), 0);
		std::vector<uint8_t>::iterator it = buffer.begin();

		it = std::copy_n(header.serialize().begin(), message_header::size, it);
		uint64_t id_ = acknowledged_message_id;

		it = std::copy_n(reinterpret_cast<uint8_t const *>(&id_), sizeof(id_), it);

		return buffer;
	}

	uint64_t acknowledge_message::get_body_size() const
	{
		return sizeof(snowflake::value);
	}
}
