#pragma once

#include <cstdint>
#include <iosfwd>

namespace pine
{
	/// <summary>
	/// Represents a unique identifier for an object.
	/// </summary>
	struct snowflake
	{
		/// <summary>
		/// Creates a new snowflake with a new generated id.
		/// </summary>
		snowflake();

		/// <summary>
		/// Creates a new snowflake from an id.
		/// </summary>
		explicit snowflake(uint64_t const& id);

		/// <summary>
		/// The value of the snowflake.
		/// </summary>
		struct
		{
			uint64_t timestamp : 42;
			uint64_t worker_id : 5;
			uint64_t process_id : 5;
			uint64_t sequence : 12;
		} value;

		/// <summary>
		/// Creates a new snowflake from an id.
		/// </summary>
		snowflake operator=(uint64_t const& id);

		bool operator !=(snowflake const& other) const;

		bool operator ==(snowflake const& other) const;

		bool operator ==(uint64_t const& other) const;

		explicit(false) constexpr operator uint64_t() const;

		/// <summary>
		/// Writes the snowflake to the output stream.
		/// </summary>
		friend std::ostream& operator<<(std::ostream& out, snowflake const& snowflake);

	private:
		/// <summary>
		/// The last millisecond a snowflake was generated.
		/// </summary>
		static uint64_t last_millisecond;
	};
}