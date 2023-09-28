#pragma once

#include <cstdint>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	/// @brief A message sent by the client to keep the connection alive.
	struct keep_alive_message : message
	{
		/// @brief Construct a keep alive message.
		keep_alive_message();

		/// @brief Load a keep alive message from given buffer.
		bool parse_body(std::vector<uint8_t> const& buffer) override;

		/// @brief Serialize the keep alive message to a buffer.
		[[nodiscard]]
		std::vector<uint8_t> serialize() const override;

		/// @brief Get the size of the body of the keep alive message.
		[[nodiscard]]
		uint64_t get_body_size() const final;
	};
}