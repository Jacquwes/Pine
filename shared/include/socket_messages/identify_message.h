#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	/// @brief A message sent by the client to identify itself.
	struct identify_message : message
	{
		/// @brief Construct an identify message with an empty username.
		identify_message();

		/// @brief Construct an identify message with the given username.
		identify_message(const std::string& name);

		/// @brief Load an identify message from given buffer.
		bool parse_body(std::vector<uint8_t> const& buffer) override;

		/// @brief Serialize the identify message to a buffer.
		[[nodiscard]]
		std::vector<uint8_t> serialize() const override;

		/// @brief Get the size of the body of the identify message.
		[[nodiscard]]
		uint64_t get_body_size() const final;


		/// @brief Check if the given username is valid.
		/// @param name Username to check.
		/// @return `true` if the username is valid, `false` otherwise.
		/// A username is valid if it is between 3 and 32 characters long.
		[[nodiscard]]
		static constexpr bool check_username(std::string_view const& name)
		{
			if (name.length() < username_min_length || name.length() > username_max_length)
				return false;

			return true;
		}

		/// @brief Username of the client.
		std::string username{};
	};
}