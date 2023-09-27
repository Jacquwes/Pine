#include "snowflake.h"

#include <chrono>
#include <cstdint>
#include <iosfwd>

namespace pine
{
	uint64_t snowflake::last_millisecond = 0;

	snowflake::snowflake()
	{
		using namespace std::chrono;
		value.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		value.worker_id = 0;
		value.process_id = 0;
		if (last_millisecond == value.timestamp)
			value.sequence++;
		else
			value.sequence = 0;
	}

	snowflake::snowflake(uint64_t const& id)
	{
		value.timestamp = id >> 22;
		value.worker_id = (id >> 17) & 0x1F;
		value.process_id = (id >> 12) & 0x1F;
		value.sequence = id & 0xFFF;
	}

	snowflake snowflake::operator=(uint64_t const& id)
	{
		*this = snowflake(id);

		return *this;
	}

	bool snowflake::operator !=(snowflake const& other) const
	{
		return !(*this == other);
	}

	bool snowflake::operator ==(snowflake const& other) const
	{
		return value.timestamp == other.value.timestamp
			&& value.worker_id == other.value.worker_id
			&& value.process_id == other.value.process_id
			&& value.sequence == other.value.sequence;
	}

	bool snowflake::operator != (uint64_t other) const
	{
		return !(static_cast<uint64_t>(*this) == other);
	}

	bool snowflake::operator ==(uint64_t other) const
	{
		return static_cast<uint64_t>(*this) == other;
	}
}