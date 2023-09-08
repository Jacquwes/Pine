#pragma once

#include <cstdint>
#include <vector>

#include "message.h"

namespace pine::socket_messages
{
	struct hello_message : message
	{
		hello_message();

		bool parse_body(std::vector<uint8_t> const& buffer) override;

		std::vector<uint8_t> serialize() const override;

		uint64_t get_body_size() const final;

		uint64_t m_version{ current_version };
	};
}