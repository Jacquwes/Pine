#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include <message.h>
#include <socket_messages/hello_message.h>

TEST(hello_message, default_constructor)
{
	pine::socket_messages::hello_message msg;
	EXPECT_EQ(msg.header.type, pine::socket_messages::message_type::HELLO_MESSAGE);
	EXPECT_EQ(msg.header.body_size, sizeof(uint64_t));
}

TEST(hello_message, parse_body)
{
	pine::socket_messages::hello_message msg;

	std::vector<uint8_t> body(0, 0);
	EXPECT_FALSE(msg.parse_body(body));

	body.resize(sizeof(uint64_t), 0);
	body[0] = 0x01;
	EXPECT_TRUE(msg.parse_body(body));

	EXPECT_EQ(msg.version, 0x01);
}

TEST(hello_message, serialize)
{
	pine::socket_messages::hello_message msg;
	msg.version = 0x01;

	std::vector<uint8_t> buffer = msg.serialize();

	EXPECT_EQ(buffer.size(), pine::socket_messages::message_header::size + sizeof(uint64_t));

	pine::socket_messages::message_header header;
	header.parse(buffer);
	EXPECT_EQ(header.type, pine::socket_messages::message_type::HELLO_MESSAGE);
	EXPECT_EQ(header.body_size, sizeof(uint64_t));

	uint64_t version{};
	std::copy_n(buffer.begin() + pine::socket_messages::message_header::size, sizeof(version), reinterpret_cast<uint8_t*>(&version));
	EXPECT_EQ(version, 0x01);
}

TEST(hello_message, get_body_size)
{
	pine::socket_messages::hello_message msg;
	EXPECT_EQ(msg.get_body_size(), sizeof(uint64_t));
}