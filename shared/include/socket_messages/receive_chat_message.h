#pragma once

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

		static constexpr bool check_author_username(std::string_view const& username)
		{
			if (username.size() < username_min_length)
				return false;

			if (username.size() > username_max_length)
				return false;

			return true;
		}

		static constexpr bool check_message_content(std::string_view const& content)
		{
			if (content.size() < chat_message_min_length)
				return false;

			if (content.size() > chat_message_max_length)
				return false;

			return true;
		}

		std::string author_username{};
		std::string message_content{};
	};
}