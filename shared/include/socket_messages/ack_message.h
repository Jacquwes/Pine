#pragma once

#include <cstdint>
#include <vector>

#include "message.h"
#include "snowflake.h"

namespace pine::socket_messages
{
	struct acknowledge_message : message
	{
		acknowledge_message();

		explicit acknowledge_message(snowflake const& id);

		bool parse_body(std::vector<uint8_t> const& buffer) override;

		[[nodiscard]] std::vector<uint8_t> serialize() const override;

		[[nodiscard]] uint64_t get_body_size() const final;

		snowflake id{};
	};
}
