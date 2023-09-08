#include "socket_messages/receive_chat_message.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	receive_chat_message::receive_chat_message()
	{
		header.type = message_type::RECEIVE_CHAT_MESSAGE;
		header.body_size = get_body_size();
	}

	bool receive_chat_message::parse_body(std::vector<uint8_t> const& buffer)
	{
		if (buffer.size() < sizeof(uint8_t) + username_min_length + sizeof(uint16_t) + chat_message_min_length)
			return false;

		if (buffer.size() > sizeof(uint8_t) + username_max_length + sizeof(uint16_t) + chat_message_max_length)
			return false;

		uint8_t username_length = buffer[0];
		if (username_length < username_min_length || username_length > username_max_length)
			return false;
		if (buffer.size() < sizeof(uint8_t) + username_length + sizeof(uint16_t) + chat_message_min_length)
			return false;

		author_username = std::string(buffer.begin() + sizeof(uint8_t), buffer.begin() + sizeof(uint8_t) + username_length);

		uint16_t message_length = *reinterpret_cast<uint16_t const*>(buffer.data() + sizeof(uint8_t) + username_length);
		if (message_length < chat_message_min_length || message_length > chat_message_max_length)
			return false;
		if (buffer.size() != sizeof(uint8_t) + username_length + sizeof(uint16_t) + message_length)
			return false;

		message_content = std::string(buffer.begin() + sizeof(uint8_t) + username_length + sizeof(uint16_t), buffer.end());

		header.body_size = get_body_size();

		return true;
	}

	std::vector<uint8_t> receive_chat_message::serialize() const
	{
		std::vector<uint8_t> buffer(message_header::size + get_body_size(), 0);
		std::vector<uint8_t>::iterator it = buffer.begin();

		it = std::copy_n(header.serialize().begin(), message_header::size, it);
		uint8_t author_username_length = author_username.size();
		it = std::copy_n(reinterpret_cast<uint8_t const*>(&author_username_length), sizeof(uint8_t), it);
		it = std::copy_n(author_username.begin(), author_username_length, it);
		uint16_t message_length = message_content.size();
		it = std::copy_n(reinterpret_cast<uint8_t const*>(&message_length), sizeof(uint16_t), it);
		it = std::copy_n(message_content.begin(), message_length, it);

		return buffer;
	}

	uint64_t receive_chat_message::get_body_size() const
	{
		return sizeof(uint8_t) + author_username.size() + sizeof(uint16_t) + message_content.size();
	}
}