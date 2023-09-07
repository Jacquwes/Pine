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
		1, // type
		0, 0, 0, 0, 0, 0, 0, 8, // body size
		1, 2, 3, 4, 5, 6, 7, 8, // id
	};

	pine::socket_messages::message_header header(buffer);

	EXPECT_EQ(header.type, pine::socket_messages::message_type::hello_message);
	EXPECT_EQ(header.body_size, 8);
	EXPECT_EQ(header.id, 0x0102030405060708ull);
}

TEST(message_header, parse)
{
	std::vector<uint8_t> buffer{
	1, // type
	0, 0, 0, 0, 0, 0, 0, 8, // body size
	1, 2, 3, 4, 5, 6, 7, 8, // id
	};

	pine::socket_messages::message_header header;
	header.parse(buffer);

	EXPECT_EQ(header.type, pine::socket_messages::message_type::hello_message);
	EXPECT_EQ(header.body_size, 8);
	EXPECT_EQ(header.id, 0x0102030405060708ull);
}

TEST(message_header, serialize)
{
	pine::socket_messages::message_header header;

	header.type = pine::socket_messages::message_type::hello_message;
	header.body_size = 8;
	header.id = 0x0102030405060708ull;

	auto buffer = header.serialize();

	EXPECT_EQ(buffer[0], 1);
	EXPECT_EQ(*(uint64_t*)(&buffer[1]), 8);
	EXPECT_EQ(*(uint64_t*)(&buffer[9]), 0x0102030405060708ull);
}