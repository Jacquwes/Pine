#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"

constexpr uint64_t ChatMessageMaxLength = 1000;

namespace SocketMessages
{
	struct SendChatMessage : Message
	{
		SendChatMessage()
		{
			header.messageType = MessageType::SendChatMessage;
			header.bodySize = GetBodySize();
		}

		bool ParseBody(std::vector<uint8_t> const& buffer) override
		{
			if (buffer.size() != GetBodySize())
				return false;

			//std::memcpy(std::bit_cast<void*>(username.data()), std::bit_cast<void*>(buffer.data()), ChatMessageMaxLength);
			return true;
		}

		std::vector<uint8_t> Serialize() const override
		{
			std::vector<uint8_t> buffer;

			buffer.append_range(header.Serialize());
			//buffer.append_range(username);

			return buffer;
		}

		uint16_t chatMessageLength;
		std::string chatMessageContent{};
		uint64_t constexpr GetBodySize() const override { return chatMessageLength + sizeof(chatMessageLength); }

	};
}