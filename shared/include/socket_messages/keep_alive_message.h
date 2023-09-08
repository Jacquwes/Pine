#pragma once

#include <cstdint>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	struct keep_alive_message : message
	{
		keep_alive_message();

		bool parse_body(std::vector<uint8_t> const& buffer) override;

		std::vector<uint8_t> serialize() const override;

		uint64_t get_body_size() const final;
	};
}