#include <gtest/gtest.h>

#include <cstdint>
#include <string_view>
#include <vector>

#include <socket_messages/send_chat_message.h>
#include <message.h>

TEST(send_chat_message, default_constructor)
{
	pine::socket_messages::send_chat_message message;
	EXPECT_EQ(message.header.type, pine::socket_messages::message_type::SEND_CHAT_MESSAGE);
	EXPECT_EQ(message.header.body_size, sizeof(uint16_t));
	EXPECT_EQ(message.message_content, "");
}

TEST(send_chat_message, parse_body)
{
	pine::socket_messages::send_chat_message message;

	// Invalid buffer size : too small
	std::vector<uint8_t> buffer(0, 0);
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid buffer size : too big
	buffer.resize(sizeof(uint16_t) + pine::chat_message_max_length + 1);
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid message length : too small
	buffer.resize(sizeof(uint16_t) + pine::chat_message_min_length);
	*reinterpret_cast<uint16_t*>(buffer.data()) = pine::chat_message_min_length - 1;
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid message length : too big
	*reinterpret_cast<uint16_t*>(buffer.data()) = pine::chat_message_max_length + 1;
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid buffer size
	*reinterpret_cast<uint16_t*>(buffer.data()) = pine::chat_message_max_length;
	EXPECT_FALSE(message.parse_body(buffer));

	// Valid buffer
	std::string_view message_content = "message";
	*reinterpret_cast<uint16_t*>(buffer.data()) = static_cast<uint16_t>(message_content.size());
	buffer.resize(sizeof(uint16_t) + message_content.size());
	std::copy_n(message_content.begin(), message_content.size(), buffer.begin() + sizeof(uint16_t));
	EXPECT_TRUE(message.parse_body(buffer));
	EXPECT_EQ(message.message_content, message_content);
}

TEST(send_chat_message, serialize)
{
	pine::socket_messages::send_chat_message message;
	message.message_content = "message";
	message.header.body_size = message.get_body_size();

	std::vector<uint8_t> buffer = message.serialize();
	std::vector <uint8_t>::iterator it = buffer.begin();

	EXPECT_EQ(buffer.size(), pine::socket_messages::message_header::size + message.get_body_size());
	EXPECT_EQ(buffer[0], static_cast<uint8_t>(pine::socket_messages::message_type::SEND_CHAT_MESSAGE));
	it += sizeof(uint8_t);
	EXPECT_EQ(*reinterpret_cast<uint64_t*>(&*it), message.header.body_size);
	it += 2 * sizeof(uint64_t);
	EXPECT_EQ(*reinterpret_cast<uint16_t*>(&*it), static_cast<uint16_t>(message.message_content.size()));
	it += sizeof(uint16_t);
	EXPECT_EQ(std::string(it, buffer.end()), message.message_content);
}

TEST(send_chat_message, get_body_size)
{
	pine::socket_messages::send_chat_message message;
	message.message_content = "message";
	EXPECT_EQ(message.get_body_size(), sizeof(uint16_t) + message.message_content.size());
}

TEST(send_chat_message, check_message_content)
{
	EXPECT_FALSE(pine::socket_messages::send_chat_message::check_message_content(""));
	EXPECT_FALSE(pine::socket_messages::send_chat_message::check_message_content(std::string(pine::chat_message_max_length + 1, 'a')));
	EXPECT_TRUE(pine::socket_messages::send_chat_message::check_message_content(std::string(pine::chat_message_max_length, 'a')));
	EXPECT_TRUE(pine::socket_messages::send_chat_message::check_message_content(std::string(pine::chat_message_min_length, 'a')));
}