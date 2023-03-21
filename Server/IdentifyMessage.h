#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"

constexpr uint64_t UsernameMaxLength = 0x20;

namespace SocketMessages
{
	struct IdentifyMessage : Message
	{
		IdentifyMessage()
		{
			header.messageType = MessageType:: IdentifyMessage;
			header.bodySize = size;
		}

		bool ParseBody(std::vector<uint8_t> const& buffer) override
		{
			if (buffer.size() != size)
				return false;

			std::memcpy(std::bit_cast<void*>(username.data()), std::bit_cast<void*>(buffer.data()), UsernameMaxLength);
			return true;
		}

		std::vector<uint8_t> Serialize() const override
		{
			std::vector<uint8_t> buffer;

			buffer.append_range(header.Serialize());
			buffer.append_range(username);

			return buffer;
		}

		std::array<uint8_t, UsernameMaxLength> username{};
		static uint64_t const size = UsernameMaxLength;
	};
}