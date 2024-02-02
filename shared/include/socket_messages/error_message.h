#pragma once

#include <cstdint>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	/// @brief Error codes for the error message.
	enum class error_code : uint8_t
	{
		/// @brief Indicates that the message layout or data is invalid.
		INVALID_MESSAGE,

		/// @brief Indicates that the message type is invalid.
		INVALID_MESSAGE_TYPE,

		/// @brief Indicates that the version of the protocol is incompatible.
		INCOMPATIBLE_API_VERSION,

		/// @brief Indicates that the message is invalid.
		WRONG_MESSAGE_LENGTH,

		/// @brief Indicates that the username length is invalid.
		WRONG_CHAT_USERNAME_LENGTH,

		/// @brief Indicates that the chat message length is invalid.
		WRONG_CHAT_MESSAGE_LENGTH,
	};

	/// @brief A message sent by the server to send an error message.
	struct error_message : message
	{
		/// @brief Construct an error message.
		error_message();

		/// @brief Construct an error message with given error code.
		explicit error_message(error_code const& error_code);

		/// @brief Load an error message from given buffer.
		bool parse_body(std::vector<uint8_t> const& buffer) override;

		/// @brief Serialize the error message to a buffer.
		[[nodiscard]] std::vector<uint8_t> serialize() const override;

		/// @brief Get the size of the body of the error message.
		[[nodiscard]] uint64_t get_body_size() const final;

		/// @brief Error code of the message.
		error_code error;
	};
}