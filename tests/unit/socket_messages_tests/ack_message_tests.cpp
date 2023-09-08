#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include <message.h>
#include <snowflake.h>
#include <socket_messages/ack_message.h>


TEST(acknowledge_message, default_constructor)
{
	pine::socket_messages::acknowledge_message msg;
	EXPECT_EQ(msg.header.type, pine::socket_messages::message_type::ACKNOWLEDGE_MESSAGE);
	EXPECT_EQ(msg.header.body_size, sizeof(pine::snowflake::value));
}

TEST(acknowledge_message, construct_from_id)
{
	pine::snowflake id;
	pine::socket_messages::acknowledge_message msg(id);

	EXPECT_EQ(msg.header.type, pine::socket_messages::message_type::ACKNOWLEDGE_MESSAGE);
	EXPECT_EQ(msg.header.body_size, sizeof(pine::snowflake::value));
	EXPECT_EQ(msg.acknowledged_message_id, id);
}

TEST(acknowledge_message, parse_body)
{
	pine::socket_messages::acknowledge_message msg;

	std::vector<uint8_t> body(0, 0);
	EXPECT_FALSE(msg.parse_body(body));

	body.resize(sizeof(pine::snowflake::value), 0);
	body[0] = 0x01;
	EXPECT_TRUE(msg.parse_body(body));

	pine::snowflake id;
	id = 0x01;
	EXPECT_EQ(msg.acknowledged_message_id, id);
}

TEST(acknowledge_message, serialize)
{
	pine::socket_messages::acknowledge_message msg;
	msg.acknowledged_message_id = 0x01;

	std::vector<uint8_t> buffer = msg.serialize();

	EXPECT_EQ(buffer.size(), pine::socket_messages::message_header::size + sizeof(pine::snowflake::value));

	pine::socket_messages::message_header header;
	header.parse(buffer);
	EXPECT_EQ(header.type, pine::socket_messages::message_type::ACKNOWLEDGE_MESSAGE);
	EXPECT_EQ(header.body_size, sizeof(pine::snowflake::value));

	uint64_t id{};
	std::copy_n(buffer.begin() + pine::socket_messages::message_header::size, sizeof(id), reinterpret_cast<uint8_t*>(&id));
	EXPECT_EQ(id, 0x01);
}

TEST(acknowledge_message, get_body_size)
{
	pine::socket_messages::acknowledge_message msg;
	EXPECT_EQ(msg.get_body_size(), sizeof(pine::snowflake::value));
}