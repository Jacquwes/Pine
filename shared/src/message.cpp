#include <algorithm>
#include <cstdint>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	message_header::message_header(std::vector<uint8_t> const& buffer)
	{
		parse(buffer);
	}

	void message_header::parse(std::vector<uint8_t> const& buffer)
	{
		std::copy_n(buffer.begin() + sizeof(type), sizeof(body_size), reinterpret_cast<uint8_t*>(&body_size));

		type = static_cast<message_type>(buffer[0]);

		uint64_t new_id = 0;
		std::copy_n(buffer.begin() + sizeof(type) + sizeof(body_size), sizeof(new_id), reinterpret_cast<uint8_t*>(&new_id));
		id = new_id;
	}

	std::vector<uint8_t> message_header::serialize() const
	{
		std::vector<uint8_t> buffer;

		buffer.push_back(static_cast<uint8_t>(type));
		buffer.resize(size);

		// copy body size
		std::copy_n(reinterpret_cast<uint8_t const*>(&body_size), sizeof(body_size), buffer.begin() + sizeof(type));

		// copy message id
		uint64_t new_id = id;

		std::copy_n(reinterpret_cast<uint8_t const*>(&new_id), sizeof(new_id), buffer.begin() + sizeof(type) + sizeof(body_size));

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