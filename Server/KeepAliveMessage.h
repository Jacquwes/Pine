#pragma once

#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"

namespace SocketMessages
{
	struct KeepAliveMessage : Message
	{
		KeepAliveMessage()
		{
			header.messageType = MessageType::KeepAliveMessage;
			header.bodySize = GetBodySize();
		}

		bool ParseBody(std::vector<uint8_t> const& buffer) override
		{
			if (buffer.size() != GetBodySize())
				return false;

			return true;
		}

		std::vector<uint8_t> Serialize() const override
		{
			return header.Serialize();
		}

		uint64_t GetBodySize() const override { return 0; }
	};
}