#include <gtest/gtest.h>

#include <cstdint>
#include <iosfwd>
#include <sstream>

#include "snowflake.h"

TEST(snowflake, default_constructor)
{
	pine::snowflake snowflake;
	EXPECT_EQ(snowflake.value.worker_id, 0);
	EXPECT_EQ(snowflake.value.process_id, 0);
	EXPECT_EQ(snowflake.value.sequence, 0);
}

TEST(snowflake, constructor)
{
	pine::snowflake snowflake(0x0000000000000001);
	EXPECT_EQ(snowflake.value.timestamp, 0);
	EXPECT_EQ(snowflake.value.worker_id, 0);
	EXPECT_EQ(snowflake.value.process_id, 0);
	EXPECT_EQ(snowflake.value.sequence, 1);
}

TEST(snowflake, assignment_operator)
{
	pine::snowflake snowflake;
	snowflake = 0x0000000000000001;
	EXPECT_EQ(snowflake.value.timestamp, 0);
	EXPECT_EQ(snowflake.value.worker_id, 0);
	EXPECT_EQ(snowflake.value.process_id, 0);
	EXPECT_EQ(snowflake.value.sequence, 1);
}

TEST(snowflake, equality_operator)
{
	pine::snowflake snowflake1(0x0000000000000001);
	pine::snowflake snowflake2(0x0000000000000001);
	EXPECT_TRUE(snowflake1 == snowflake2);
}

TEST(snowflake, inequality_operator)
{
	pine::snowflake snowflake1(0x0000000000000001);
	pine::snowflake snowflake2(0x0000000000000002);
	EXPECT_TRUE(snowflake1 != snowflake2);
}

TEST(snowflake, equality_operator_with_uint64_t)
{
	pine::snowflake snowflake(0x0000000000000001);
	EXPECT_TRUE(snowflake == (uint64_t)0x0000000000000001);
}

TEST(snowflake, inequality_operator_with_uint64_t)
{
	pine::snowflake snowflake(0x0000000000000001);
	EXPECT_TRUE(snowflake != (uint64_t)0x0000000000000002);
}

TEST(snowflake, cast_to_uint64_t)
{
	pine::snowflake snowflake(0x0000000000000001);
	EXPECT_EQ((uint64_t)snowflake, 0x0000000000000001);
}

TEST(snowflake, stream_operator)
{
	pine::snowflake snowflake(0x0000000000000001);
	std::stringstream stream;
	stream << snowflake;
	EXPECT_EQ(stream.str(), "1");
}
