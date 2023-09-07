#pragma once

#include <cstdint>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	enum class error_code : uint8_t
	{
		INVALID_MESSAGE,
		INVALID_MESSAGE_TYPE,
		INCOMPATIBLE_API_VERSION,
		WRONG_MESSAGE_LENGTH,
		WRONG_CHAT_USERNAME_LENGTH,
		WRONG_CHAT_MESSAGE_LENGTH,
	};

	struct error_message : message
	{
		error_message();

		explicit error_message(error_code const& error_code);

		bool parse_body(std::vector<uint8_t> const& buffer) override;

		[[nodiscard]] std::vector<uint8_t> serialize() const override;

		[[nodiscard]] uint64_t get_body_size() const final;

		error_code m_error_code;
	};
}