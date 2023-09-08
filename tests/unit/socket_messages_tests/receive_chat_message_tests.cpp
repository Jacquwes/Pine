#include <gtest/gtest.h>

#include <cstdint>
#include <string_view>
#include <vector>

#include <socket_messages/receive_chat_message.h>
#include <message.h>

TEST(receive_chat_message, default_constructor)
{
	pine::socket_messages::receive_chat_message message;

	EXPECT_EQ(message.header.type, pine::socket_messages::message_type::RECEIVE_CHAT_MESSAGE);
	EXPECT_EQ(message.header.body_size, sizeof(uint8_t) + sizeof(uint16_t));
	EXPECT_EQ(message.author_username, "");
	EXPECT_EQ(message.message_content, "");
}

TEST(receive_chat_message, parse_body)
{
	pine::socket_messages::receive_chat_message message;

	// Invalid buffer size : too small
	std::vector<uint8_t> buffer(0, 0);
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid buffer size : too big
	buffer.resize(sizeof(uint8_t) + pine::username_max_length + sizeof(uint16_t) + pine::chat_message_max_length + 1);
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid username length : too small
	buffer.resize(sizeof(uint8_t) + pine::username_min_length + sizeof(uint16_t) + pine::chat_message_min_length);
	buffer[0] = pine::username_min_length - 1;
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid username length : too big
	buffer[0] = pine::username_max_length + 1;
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid message length : too small
	buffer[0] = pine::username_min_length;
	*reinterpret_cast<uint16_t*>(buffer.data() + sizeof(uint8_t) + pine::username_min_length) =
		pine::chat_message_min_length - 1;
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid message length : too big
	*reinterpret_cast<uint16_t*>(buffer.data() + sizeof(uint8_t) + pine::username_min_length) =
		pine::chat_message_max_length + 1;
	EXPECT_FALSE(message.parse_body(buffer));

	// Invalid buffer size
	*reinterpret_cast<uint16_t*>(buffer.data() + sizeof(uint8_t) + pine::username_min_length) =
		pine::chat_message_min_length;
	buffer.resize(sizeof(uint8_t) + pine::username_min_length + sizeof(uint16_t) + pine::chat_message_min_length + 1);
	EXPECT_FALSE(message.parse_body(buffer));

	// Valid buffer
	std::string_view username = "username";
	std::string_view message_content = "message";
	buffer.resize(sizeof(uint8_t) + username.size() + sizeof(uint16_t) + message_content.size());
	buffer[0] = username.size();
	std::copy_n(username.begin(), username.size(), buffer.begin() + sizeof(uint8_t));
	*reinterpret_cast<uint16_t*>(buffer.data() + sizeof(uint8_t) + username.size()) = message_content.size();
	std::copy_n(message_content.begin(), message_content.size(), buffer.begin() + sizeof(uint8_t) + username.size() + sizeof(uint16_t));
	EXPECT_TRUE(message.parse_body(buffer));
}

TEST(receive_chat_message, serialize)
{
	pine::socket_messages::receive_chat_message msg;
	msg.author_username = "username";
	msg.message_content = "message";
	msg.header.body_size = msg.get_body_size();

	std::vector<uint8_t> buffer = msg.serialize();

	EXPECT_EQ(buffer.size(), pine::socket_messages::message_header::size + msg.get_body_size());
	EXPECT_EQ(buffer[0], msg.header.type);
	EXPECT_EQ(buffer[1], msg.header.body_size);	
	EXPECT_EQ(buffer[pine::socket_messages::message_header::size], msg.author_username.size());

	auto it = buffer.begin() + pine::socket_messages::message_header::size + sizeof(uint8_t);
	EXPECT_TRUE(std::equal(msg.author_username.begin(), msg.author_username.end(), it));

	it += msg.author_username.size();
	EXPECT_EQ(*reinterpret_cast<uint16_t const*>(&*it), msg.message_content.size());

	it += sizeof(uint16_t);
	EXPECT_TRUE(std::equal(msg.message_content.begin(), msg.message_content.end(), it));

	it += msg.message_content.size();
	EXPECT_EQ(it, buffer.end());
}

TEST(receive_chat_message, get_body_size)
{
	pine::socket_messages::receive_chat_message msg;

	EXPECT_EQ(msg.get_body_size(), sizeof(uint8_t) + sizeof(uint16_t));

	msg.author_username = "username";
	msg.message_content = "message";

	EXPECT_EQ(msg.get_body_size(), sizeof(uint8_t) + msg.author_username.size() + sizeof(uint16_t) + msg.message_content.size());
}

TEST(receive_chat_message, check_author_username)
{
	EXPECT_FALSE(pine::socket_messages::receive_chat_message::check_author_username(""));

	std::string username(pine::username_min_length - 1, 'a');
	EXPECT_FALSE(pine::socket_messages::receive_chat_message::check_author_username(username));

	username.resize(pine::username_max_length + 1);
	EXPECT_FALSE(pine::socket_messages::receive_chat_message::check_author_username(username));

	username.resize(pine::username_min_length);
	EXPECT_TRUE(pine::socket_messages::receive_chat_message::check_author_username(username));

	username.resize(pine::username_max_length);
	EXPECT_TRUE(pine::socket_messages::receive_chat_message::check_author_username(username));
}

TEST(receive_chat_message, check_message_content)
{
	EXPECT_FALSE(pine::socket_messages::receive_chat_message::check_message_content(""));

	std::string message(pine::chat_message_min_length - 1, 'a');
	EXPECT_FALSE(pine::socket_messages::receive_chat_message::check_message_content(message));

	message.resize(pine::chat_message_max_length + 1);
	EXPECT_FALSE(pine::socket_messages::receive_chat_message::check_message_content(message));

	message.resize(pine::chat_message_min_length);
	EXPECT_TRUE(pine::socket_messages::receive_chat_message::check_message_content(message));

	message.resize(pine::chat_message_max_length);
	EXPECT_TRUE(pine::socket_messages::receive_chat_message::check_message_content(message));
}