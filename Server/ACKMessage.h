#pragma once

#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"
#include "Snowflake.h"

namespace SocketMessages
{
	struct AcknowledgeMessage : Message
	{
		AcknowledgeMessage()
		{
			header.messageType = MessageType::AcknowledgeMessage;
			header.bodySize = GetBodySize();
		}

		explicit AcknowledgeMessage(Snowflake const& id)
			: m_messageId(id)
		{
			header.messageType = MessageType::AcknowledgeMessage;
			header.bodySize = GetBodySize();
		}

		bool ParseBody(std::vector<uint8_t> const& buffer) override
		{
			if (buffer.size() != GetBodySize())
				return false;

			std::memcpy(std::bit_cast<void*>(&m_messageId), std::bit_cast<void*>(buffer.data()), sizeof(Snowflake::Value));
			return true;
		}

		std::vector<uint8_t> Serialize() const override
		{
			std::vector<uint8_t> buffer(MessageHeader::size + GetBodySize(), 0);
			std::vector<uint8_t> headerBuffer = header.Serialize();

			std::memcpy(&buffer[0], &headerBuffer[0], MessageHeader::size);
			uint64_t id = m_messageId;
			std::memcpy(&buffer[SocketMessages::MessageHeader::size], &id, sizeof(id));

			return buffer;
		}

		uint64_t GetBodySize() const final { return sizeof(Snowflake::Value); }

		[[nodiscard]] constexpr Snowflake const& GetAcknowledgedMessageId() const { return m_messageId; }
		void SetAcknowledgedMessageId(Snowflake const& acknowledgedMessageId) { m_messageId = acknowledgedMessageId; }

	private:
		Snowflake m_messageId{};
	};
}