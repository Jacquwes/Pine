#include <gtest/gtest.h>

#include "message.h"

TEST(message_header, default_constructor)
{
	pine::socket_messages::message_header header;
	EXPECT_EQ(header.type, pine::socket_messages::message_type::invalid_message);
	EXPECT_EQ(header.body_size, 0);
	EXPECT_NE(header.id, 0ull);
}

TEST(message_header, construct_from_buffer)
{
	std::vector<uint8_t> buffer{
		pine::socket_messages::message_type::hello_message, // type
		1, 0, 0, 0, 0, 0, 0, 0, // body size
		1, 0, 0, 0, 0, 0, 0, 0, // id
	};

	pine::socket_messages::message_header header(buffer);

	EXPECT_EQ(header.type, pine::socket_messages::message_type::hello_message);
	EXPECT_EQ(header.body_size, 1);
	EXPECT_EQ(header.id, 1ull);
}

TEST(message_header, parse)
{
	std::vector<uint8_t> buffer{
		pine::socket_messages::message_type::hello_message, // type
		1, 0, 0, 0, 0, 0, 0, 0, // body size
		1, 0, 0, 0, 0, 0, 0, 0, // id
	};

	pine::socket_messages::message_header header;
	header.parse(buffer);

	EXPECT_EQ(header.type, pine::socket_messages::message_type::hello_message);
	EXPECT_EQ(header.body_size, 1);
	EXPECT_EQ(header.id, 1ull);
}

TEST(message_header, serialize)
{
	pine::socket_messages::message_header header;

	header.type = pine::socket_messages::message_type::hello_message;
	header.body_size = 1;
	header.id = 1ull;

	auto buffer = header.serialize();

	EXPECT_EQ(buffer[0], pine::socket_messages::message_type::hello_message);
	EXPECT_EQ(*(uint64_t*)(&buffer[1]), 1);
	EXPECT_EQ(*(uint64_t*)(&buffer[9]), 1ull);
}