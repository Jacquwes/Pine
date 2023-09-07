#pragma once

#include <cstdint>
#include <ostream>

namespace pine
{
	struct snowflake
	{
		snowflake();

		explicit snowflake(uint64_t const& id);

		struct
		{
			uint64_t timestamp : 42;
			uint64_t worker_id : 5;
			uint64_t process_id : 5;
			uint64_t sequence : 12;
		} value;

		snowflake operator=(uint64_t const& id);

		bool operator !=(snowflake const& other) const;

		bool operator ==(snowflake const& other) const;

		bool operator ==(uint64_t const& other) const;

		explicit(false) constexpr operator uint64_t() const;

		friend std::ostream& operator<<(std::ostream& out, snowflake const& snowflake);

		static uint64_t last_millisecond;
	};
}