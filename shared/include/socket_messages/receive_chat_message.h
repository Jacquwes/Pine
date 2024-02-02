#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	/// @brief A message sent by the server to send a chat message.
	struct receive_chat_message : message
	{
		/// @brief Construct a receive chat message with an empty message content.
		receive_chat_message();

		/// @brief Load a receive chat message with from given buffer.
		bool parse_body(std::vector<uint8_t> const& buffer) override;

		/// @brief Serialize the receive chat message to a buffer.
		[[nodiscard]]
		std::vector<uint8_t> serialize() const override;

		/// @brief Get the size of the body of the receive chat message.
		[[nodiscard]]
		uint64_t get_body_size() const final;

		/// @brief Check if the given author username is valid.
		/// @param username Author username to check.
		/// @return `true` if the author username is valid, `false` otherwise.
		/// An author username is valid if it is between 1 and 32 characters long.
		[[nodiscard]]
		static constexpr bool check_author_username(std::string_view const& username)
		{
			if (username.size() < username_min_length)
				return false;

			if (username.size() > username_max_length)
				return false;

			return true;
		}

		/// @brief Check if the given message content is valid.
		/// @param content Message content to check.
		/// @return `true` if the message content is valid, `false` otherwise.
		/// A message content is valid if it is between 1 and 2000 characters long.
		[[nodiscard]]
		static constexpr bool check_message_content(std::string_view const& content)
		{
			if (content.size() < chat_message_min_length)
				return false;

			if (content.size() > chat_message_max_length)
				return false;

			return true;
		}

		/// @brief Author username of the message.
		std::string author_username{};

		/// @brief Content of the message.
		std::string message_content{};
	};
}