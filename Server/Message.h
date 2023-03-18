#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace SocketMessages
{
	enum class MessageType : uint8_t
	{
		Invalid,
		Hello,
		Login
	};

	struct MessageHeader
	{
		MessageHeader() = default;
		explicit MessageHeader(std::vector<uint8_t> const& buffer);
		void Parse(std::vector<uint8_t> const& buffer);
		std::vector<uint8_t> Serialize() const;

		MessageType messageType{};
		uint64_t bodySize{};

		static uint64_t constexpr size = sizeof(messageType) + sizeof(bodySize);
	};

	struct Message : std::enable_shared_from_this<Message>
	{
		virtual ~Message() = default;
		virtual bool ParseBody(std::vector<uint8_t> const& buffer);
		virtual std::vector<uint8_t> Serialize() const;
			
		MessageHeader header{};
	};
}