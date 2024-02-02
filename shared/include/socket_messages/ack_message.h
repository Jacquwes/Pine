#pragma once

#include <cstdint>
#include <vector>

#include "message.h"
#include "snowflake.h"

namespace pine::socket_messages
{
	/// @brief A message sent by the server to acknowledge a message.
	struct acknowledge_message : message
	{
		/// @brief Construct an acknowledge message.
		acknowledge_message();

		/// @brief Construct an acknowledge message with given acknowledged message id.
		explicit acknowledge_message(snowflake const& id);

		/// @brief Load an acknowledge message from given buffer.
		bool parse_body(std::vector<uint8_t> const& buffer) override;

		/// @brief Serialize the acknowledge message to a buffer.
		[[nodiscard]] std::vector<uint8_t> serialize() const override;

		/// @brief Get the size of the body of the acknowledge message.
		[[nodiscard]] uint64_t get_body_size() const final;

		/// @brief Id of the acknowledged message.
		snowflake acknowledged_message_id{};
	};
}
