#pragma once

#include <cstdint>
#include <iostream>

namespace pine
{
	/// @brief A snowflake is a unique id that is used to identify objects.
	struct snowflake
	{
		/// @brief Create a new snowflake with a new generated id.
		snowflake();

		/// @brief Creates a new snowflake with the given id.
		explicit snowflake(uint64_t const& id);

		/// @brief The value of the snowflake.
		struct
		{
			/// @brief The timestamp the snowflake was generated.
			uint64_t timestamp : 42;

			/// @brief The worker id that generated the snowflake.
			uint64_t worker_id : 5;

			/// @brief The process id that generated the snowflake.
			/// @details This is used to identify the process that
			/// generated the snowflake.
			uint64_t process_id : 5;

			/// @brief The sequence number of the snowflake.
			/// @details This is used to prevent collisions.
			/// The sequence number is incremented every time
			/// a snowflake is generated in the same millisecond.
			uint64_t sequence : 12;
		} value;

		/// @brief Create a new snowflake with the given id.
		snowflake operator=(uint64_t const& id);

		/// @brief Compare two snowflakes.
		bool operator !=(snowflake const& other) const;

		/// @brief Compare two snowflakes.
		bool operator ==(snowflake const& other) const;

		/// @brief Compare a snowflake with an id.
		bool operator ==(uint64_t const& other) const;

		/// @brief Convert the snowflake to an id.
		constexpr operator uint64_t() const
		{
			return (value.timestamp << 22)
				| (value.worker_id << 17)
				| (value.process_id << 12)
				| value.sequence;
		}

		/// @brief Write the snowflake to the given output stream.
		friend std::ostream& operator << (std::ostream& out, pine::snowflake const& snowflake)
		{
			return out << static_cast<uint64_t>(snowflake);
		}

	private:
		/// @brief The last millisecond a snowflake was generated.
		static uint64_t last_millisecond;
	};
}
