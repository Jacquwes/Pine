#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <vector>

#include <message.h>
#include <socket_messages/identify_message.h>

TEST(identify_message, default_constructor)
{
	pine::socket_messages::identify_message msg;
	EXPECT_EQ(msg.header.type, pine::socket_messages::message_type::IDENTIFY_MESSAGE);
	EXPECT_EQ(msg.header.body_size, sizeof(uint8_t));
}

TEST(identify_message, construct_from_name)
{
	std::string name = "test";
	pine::socket_messages::identify_message msg(name);

	EXPECT_EQ(msg.header.type, pine::socket_messages::message_type::IDENTIFY_MESSAGE);
	EXPECT_EQ(msg.header.body_size, sizeof(uint8_t) + name.size());
	EXPECT_EQ(msg.username, name);
}

TEST(identify_message, parse_body)
{
	pine::socket_messages::identify_message msg;
	std::string name = "test";

	std::vector<uint8_t> body(0, 0);
	EXPECT_FALSE(msg.parse_body(body));

	body.resize(sizeof(uint8_t) + name.size(), 0);
	body[0] = name.size();
	std::copy_n(name.begin(), name.size(), body.begin() + sizeof(uint8_t));
	EXPECT_TRUE(msg.parse_body(body));

	EXPECT_EQ(msg.username, name);
}

TEST(identify_message, serialize)
{
	pine::socket_messages::identify_message msg;
	msg.username = "test";

	std::vector<uint8_t> buffer = msg.serialize();

	EXPECT_EQ(buffer.size(), pine::socket_messages::message_header::size + sizeof(uint8_t) + msg.username.size());

	pine::socket_messages::message_header header;
	header.parse(buffer);
	EXPECT_EQ(header.type, pine::socket_messages::message_type::IDENTIFY_MESSAGE);
	EXPECT_EQ(header.body_size, sizeof(uint8_t) + msg.username.size());

	uint8_t username_size{};
	std::copy_n(buffer.begin() + pine::socket_messages::message_header::size, sizeof(username_size), &username_size);
	EXPECT_EQ(username_size, msg.username.size());

	std::string username(buffer.begin() + pine::socket_messages::message_header::size + sizeof(username_size), buffer.end());
	EXPECT_EQ(username, msg.username);
}

TEST(identify_message, get_body_size)
{
	pine::socket_messages::identify_message msg;
	msg.username = "test";
	EXPECT_EQ(msg.get_body_size(), sizeof(uint8_t) + msg.username.size());
}

TEST(identify_message, check_username)
{
	EXPECT_TRUE(pine::socket_messages::identify_message::check_username("test"));
	EXPECT_FALSE(pine::socket_messages::identify_message::check_username(""));
	EXPECT_FALSE(pine::socket_messages::identify_message::check_username("0102030405060708091011121314151617"));
}