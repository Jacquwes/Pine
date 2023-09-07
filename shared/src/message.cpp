#include <cstdint>
#include <vector>

#include "message.h"
#include "snowflake.h"

namespace pine::socket_messages
{
	message_header::message_header(std::vector<uint8_t> const& buffer)
	{
		parse(buffer);
	}

	void message_header::parse(std::vector<uint8_t> const& buffer)
	{
		std::memcpy(&body_size, &buffer[1], sizeof(body_size));

		type = static_cast<message_type>(buffer[0]);


		uint64_t new_id = 0;
		std::memcpy(&id, &buffer[sizeof(type) + sizeof(body_size)], sizeof(new_id));
		id = snowflake{ new_id };
	}

	std::vector<uint8_t> message_header::serialize() const
	{
		std::vector<uint8_t> buffer;

		buffer.push_back(static_cast<uint8_t>(type));
		buffer.resize(size);

		// copy body size
		std::memcpy(&buffer[sizeof(type)], &body_size, sizeof(body_size));

		// copy message id
		uint64_t new_id = id;

		std::memcpy(&buffer[sizeof(type) + sizeof(body_size)], &new_id, sizeof(new_id));
		
		return buffer;
	}

	bool message::parse_body(std::vector<uint8_t> const& buffer)
	{
		return false;
	}

	std::vector<uint8_t> message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size, 0);
		return buffer;
	}

	uint64_t message::get_body_size() const
	{
		return 0;
	}
}