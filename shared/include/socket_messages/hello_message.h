#pragma once

#include <cstdint>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	/// @brief A message sent by the client and the server to confirm the connection.
	struct hello_message : message
	{
		/// @brief Construct a hello message.
		hello_message();

		/// @brief Load a hello message from given buffer.
		bool parse_body(std::vector<uint8_t> const& buffer) override;

		/// @brief Serialize the hello message to a buffer.
		[[nodiscard]]
		std::vector<uint8_t> serialize() const override;

		/// @brief Get the size of the body of the hello message.
		[[nodiscard]]
		uint64_t get_body_size() const final;

		/// @brief Version of the protocol.
		uint64_t version{ current_version };
	};
}