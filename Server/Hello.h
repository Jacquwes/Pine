#pragma once

#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"

constexpr uint64_t CURRENT_VERSION = 0x0;

namespace SocketMessages
{
	struct Hello : Message
	{
		Hello()
		{
			header.messageType = MessageType::Hello;
			header.bodySize = size;
		}
		
		uint64_t version = CURRENT_VERSION;
			
		static uint64_t const size = sizeof(version);

		bool ParseBody(std::vector<uint8_t> const& buffer) override
		{
			if (buffer.size() != size)
				return false;

			std::memcpy(std::bit_cast<void*>(&version), std::bit_cast<void*>(buffer.data()), sizeof(version));
			return true;
		}
		
		std::vector<uint8_t> Serialize() const override
		{
			std::vector<uint8_t> buffer;
			buffer.append_range(header.Serialize());

			buffer.resize(17);
			std::memcpy(&buffer[9], &version, sizeof(version));

			return buffer;
		}
	};
}