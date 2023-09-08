#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	struct identify_message : message
	{
		identify_message();
		identify_message(const std::string& name);

		bool parse_body(std::vector<uint8_t> const& buffer) override;

		std::vector<uint8_t> serialize() const override;

		uint64_t get_body_size() const final;

		constexpr bool check_username(std::string_view const& username) const;

		std::string username{};
	};
}