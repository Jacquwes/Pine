#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include <message.h>
#include <socket_messages/error_message.h>

TEST(error_message, default_constructor)
{
	pine::socket_messages::error_message msg;
	EXPECT_EQ(msg.header.type, pine::socket_messages::message_type::ERROR_MESSAGE);
	EXPECT_EQ(msg.header.body_size, sizeof(pine::socket_messages::error_code));
}

TEST(error_message, construct_from_error_code)
{
	pine::socket_messages::error_message msg(pine::socket_messages::error_code::INVALID_MESSAGE);

	EXPECT_EQ(msg.header.type, pine::socket_messages::message_type::ERROR_MESSAGE);
	EXPECT_EQ(msg.header.body_size, sizeof(pine::socket_messages::error_code));
	EXPECT_EQ(msg.error, pine::socket_messages::error_code::INVALID_MESSAGE);
}

TEST(error_message, parse_body)
{
	pine::socket_messages::error_message msg;

	std::vector<uint8_t> body(0, 0);
	EXPECT_FALSE(msg.parse_body(body));

	body.resize(sizeof(pine::socket_messages::error_code), 0);
	body[0] = 0x01;
	EXPECT_TRUE(msg.parse_body(body));

	EXPECT_EQ(msg.error, pine::socket_messages::error_code::INVALID_MESSAGE_TYPE);
}

TEST(error_message, serialize)
{
	pine::socket_messages::error_message msg;
	msg.error = pine::socket_messages::error_code::INVALID_MESSAGE_TYPE;

	std::vector<uint8_t> buffer = msg.serialize();

	EXPECT_EQ(buffer.size(), pine::socket_messages::message_header::size + sizeof(pine::socket_messages::error_code));

	pine::socket_messages::message_header header;
	header.parse(buffer);
	EXPECT_EQ(header.type, pine::socket_messages::message_type::ERROR_MESSAGE);
	EXPECT_EQ(header.body_size, sizeof(pine::socket_messages::error_code));

	uint8_t error{};
	std::copy_n(buffer.begin() + pine::socket_messages::message_header::size, sizeof(error), &error);
	EXPECT_EQ(error, 0x01);
}

TEST(error_message, get_body_size)
{
	pine::socket_messages::error_message msg;
	EXPECT_EQ(msg.get_body_size(), sizeof(pine::socket_messages::error_code));
}