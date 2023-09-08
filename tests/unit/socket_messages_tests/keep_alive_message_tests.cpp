#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include <message.h>
#include <socket_messages/keep_alive_message.h>

TEST(keep_alive_message_tests, constructor)
{
	pine::socket_messages::keep_alive_message message{};

	EXPECT_EQ(message.header.type, pine::socket_messages::message_type::KEEP_ALIVE_MESSAGE);
	EXPECT_EQ(message.header.body_size, 0);
}

TEST(keep_alive_message_tests, parse_body)
{
	pine::socket_messages::keep_alive_message message{};

	std::vector<uint8_t> buffer(0, 0);
	EXPECT_TRUE(message.parse_body(buffer));

	buffer.resize(1, 0);
	EXPECT_FALSE(message.parse_body(buffer));
}

TEST(keep_alive_message_tests, serialize)
{
	pine::socket_messages::keep_alive_message message{};

	std::vector<uint8_t> buffer = message.serialize();

	EXPECT_EQ(buffer.size(), pine::socket_messages::message_header::size);

	pine::socket_messages::message_header header;
	header.parse(buffer);
	EXPECT_EQ(header.type, pine::socket_messages::message_type::KEEP_ALIVE_MESSAGE);
	EXPECT_EQ(header.body_size, 0);
}

TEST(keep_alive_message_tests, get_body_size)
{
	pine::socket_messages::keep_alive_message message{};
	EXPECT_EQ(message.get_body_size(), 0);
}
