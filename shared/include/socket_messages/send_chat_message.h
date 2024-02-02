#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	/// @brief A message sent by the client to send a chat message.
	struct send_chat_message : message
	{
		/// @brief Construct a send chat message with an empty message content.
		send_chat_message();

		/// @brief Load a send chat message with from given buffer.
		bool parse_body(std::vector<uint8_t> const& buffer) override;

		/// @brief Serialize the send chat message to a buffer.
		[[nodiscard]]
		std::vector<uint8_t> serialize() const override;

		/// @brief Get the size of the body of the send chat message.
		[[nodiscard]]
		uint64_t get_body_size() const final;

		/// @brief Check if the given message content is valid.
		/// @param content Message content to check.
		/// @return `true` if the message content is valid, `false` otherwise.
		/// A message content is valid if it is between 1 and 2000 characters long.
		[[nodiscard]]
		static constexpr bool check_message_content(std::string_view const& content)
		{
			if (content.size() < chat_message_min_length || content.size() > chat_message_max_length)
				return false;

			return true;
		}

		/// @brief Content of the message.
		std::string message_content{};
	};
}