#pragma once

#include <bit>
#include <cstdint>
#include <string>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	struct receive_chat_message : message
	{
		receive_chat_message();

		bool parse_body(std::vector<uint8_t> const& buffer) override;

		std::vector<uint8_t> serialize() const override;

		uint64_t get_body_size() const final;

		constexpr bool check_author_username(std::string_view const& author_username);
		constexpr bool check_message_content(std::string_view const& message_content);

		std::string author_username{};
		std::string message_content{};
	};
}