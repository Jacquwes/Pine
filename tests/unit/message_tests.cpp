#include <gtest/gtest.h>

#include <vector>

#include "message.h"
#include "socket_messages.h"

TEST(message_header, default_constructor)
{
	pine::socket_messages::message_header header;
	EXPECT_EQ(header.type, pine::socket_messages::message_type::INVALID_MESSAGE);
	EXPECT_EQ(header.body_size, 0);
	EXPECT_NE(header.id, 0ull);
}

TEST(message_header, construct_from_buffer)
{
	std::vector<uint8_t> buffer{
		pine::socket_messages::message_type::HELLO_MESSAGE, // type
		1, 0, 0, 0, 0, 0, 0, 0, // body size
		1, 0, 0, 0, 0, 0, 0, 0, // id
	};

	pine::socket_messages::message_header header(buffer);

	EXPECT_EQ(header.type, pine::socket_messages::message_type::HELLO_MESSAGE);
	EXPECT_EQ(header.body_size, 1);
	EXPECT_EQ(header.id, 1ull);
}

TEST(message_header, parse)
{
	std::vector<uint8_t> buffer{
		pine::socket_messages::message_type::HELLO_MESSAGE, // type
		1, 0, 0, 0, 0, 0, 0, 0, // body size
		1, 0, 0, 0, 0, 0, 0, 0, // id
	};

	pine::socket_messages::message_header header;
	header.parse(buffer);

	EXPECT_EQ(header.type, pine::socket_messages::message_type::HELLO_MESSAGE);
	EXPECT_EQ(header.body_size, 1);
	EXPECT_EQ(header.id, 1ull);
}

TEST(message_header, serialize)
{
	pine::socket_messages::message_header header;

	header.type = pine::socket_messages::message_type::HELLO_MESSAGE;
	header.body_size = 1;
	header.id = 1ull;

	auto buffer = header.serialize();

	EXPECT_EQ(buffer[0], pine::socket_messages::message_type::HELLO_MESSAGE);
	EXPECT_EQ(*(uint64_t*)(&buffer[1]), 1);
	EXPECT_EQ(*(uint64_t*)(&buffer[9]), 1ull);
}

TEST(message, default_constructor)
{
	pine::socket_messages::message msg;
	EXPECT_EQ(msg.header.type, pine::socket_messages::message_type::INVALID_MESSAGE);
	EXPECT_EQ(msg.header.body_size, 0);
	EXPECT_NE(msg.header.id, 0ull);
}

TEST(message, get_body_size)
{
	pine::socket_messages::message msg;
	EXPECT_EQ(msg.get_body_size(), 0);
}

TEST(message, parse_body)
{
	pine::socket_messages::message msg;
	EXPECT_FALSE(msg.parse_body({}));
}

TEST(message, serialize)
{
	pine::socket_messages::message msg;
	auto buffer = msg.serialize();
	EXPECT_EQ(buffer.size(), pine::socket_messages::message_header::size);
}

