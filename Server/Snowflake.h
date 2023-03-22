#pragma once

#include <chrono>
#include <cstdint>
#include <ostream>
#include <type_traits>

struct Snowflake
{
	Snowflake()
	{
		using namespace std::chrono;
		Value.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		Value.workerId = 0;
		Value.processId = 0;
		if (lastMillisecond == Value.timestamp)
			Value.sequence++;
		else
			Value.sequence = 0;
	}

	explicit Snowflake(uint64_t const& id)
	{
		Value.timestamp = id >> 22;
		Value.workerId = (id >> 17) & 0x1F;
		Value.processId = (id >> 12) & 0x1F;
		Value.sequence = id & 0xFFF;
	}

	struct
	{
		uint64_t timestamp : 42;
		uint64_t workerId : 5;
		uint64_t processId : 5;
		uint64_t sequence : 12;
	} Value;

	bool operator ==(Snowflake const& other) const
	{
		return Value.timestamp == other.Value.timestamp
			&& Value.workerId == other.Value.workerId
			&& Value.processId == other.Value.processId
			&& Value.sequence == other.Value.sequence;
	}

	template <typename T>
		requires std::is_integral_v<T>
	explicit(false) constexpr operator T() const
	{
		return static_cast<T>(Value.timestamp) << 22
			| static_cast<T>(Value.workerId) << 17
			| static_cast<T>(Value.processId) << 12
			| static_cast<T>(Value.sequence);
	}

	friend std::ostream& operator<<(std::ostream& out, Snowflake const& snowflake)
	{
		return out << static_cast<uint64_t>(snowflake);
	}

	static inline uint64_t lastMillisecond = 0;
};
