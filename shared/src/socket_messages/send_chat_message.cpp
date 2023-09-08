#include "socket_messages/send_chat_message.h"

#include <cstdint>
#include <string>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	send_chat_message::send_chat_message()
	{
		header.type = message_type::SEND_CHAT_MESSAGE;
		header.body_size = get_body_size();
	}

	bool send_chat_message::parse_body(std::vector<uint8_t> const& buffer)
	{
		if (buffer.size() < sizeof(uint16_t) + chat_message_min_length
			|| buffer.size() > sizeof(uint16_t) + chat_message_max_length)
			return false;

		uint16_t message_length = *reinterpret_cast<uint16_t const*>(buffer.data());
		if (message_length < chat_message_min_length || message_length > chat_message_max_length)
			return false;
		if (buffer.size() != sizeof(uint16_t) + message_length)
			return false;

		message_content = std::string(buffer.begin() + sizeof(uint16_t), buffer.end());

		header.body_size = get_body_size();

		return true;
	}

	std::vector<uint8_t> send_chat_message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size + sizeof(uint16_t) + message_content.size());
		std::vector<uint8_t>::iterator it = buffer.begin();

		it = std::copy_n(header.serialize().begin(), message_header::size, it);
		uint16_t message_length = static_cast<uint16_t>(message_content.size());
		it = std::copy_n(reinterpret_cast<uint8_t const*>(&message_length), sizeof(uint16_t), it);
		it = std::copy_n(message_content.begin(), message_content.size(), it);

		return buffer;
	}

	constexpr uint64_t send_chat_message::get_body_size() const
	{
		return sizeof(uint16_t) + message_content.size();
	}
}