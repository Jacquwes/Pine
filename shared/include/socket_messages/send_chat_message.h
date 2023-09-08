#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	struct send_chat_message : message
	{
		send_chat_message();

		bool parse_body(std::vector<uint8_t> const& buffer) override;

		std::vector<uint8_t> serialize() const override;

		uint64_t constexpr get_body_size() const final;

		static constexpr bool check_message_content(std::string_view const& content)
		{
			if (content.size() < chat_message_min_length || content.size() > chat_message_max_length)
				return false;

			return true;
		}

		std::string message_content{};
	};
}