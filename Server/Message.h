#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Snowflake.h"

namespace SocketMessages
{
	enum class MessageType : uint8_t
	{
		InvalidMessage,
		AcknowledgeMessage,
		HelloMessage,
		IdentifyMessage,
		KeepAliveMessage,
		SendChatMessage,
		ReceiveChatMessage,
		ErrorMessage,
	};

	struct MessageHeader
	{
		MessageHeader() = default;
		explicit MessageHeader(std::vector<uint8_t> const& buffer);
		void Parse(std::vector<uint8_t> const& buffer);
		std::vector<uint8_t> Serialize() const;


		MessageType messageType{};
		uint64_t bodySize{};
		Snowflake messageId{};

		static uint64_t constexpr size = sizeof(messageType) + sizeof(uint64_t) + sizeof(messageId.Value);
	};

	struct Message : std::enable_shared_from_this<Message>
	{
		virtual ~Message() = default;
		virtual uint64_t GetBodySize() const;
		virtual bool ParseBody(std::vector<uint8_t> const& buffer);
		virtual std::vector<uint8_t> Serialize() const;
			
		MessageHeader header{};
	};
}