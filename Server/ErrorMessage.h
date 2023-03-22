#pragma once

#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"

namespace SocketMessages
{
	enum class ErrorCode : uint8_t
	{
		InvalidMessage,
		InvalidMessageType,
		IncompatibleAPIVersion,
		WrongMessageLength,
		WrongChatUsernameLength,
		WrongChatMessageLength,
	};

	struct ErrorMessage : Message
	{
		ErrorMessage()
		{
			header.messageType = MessageType::ErrorMessage;
			header.bodySize = GetBodySize();
		}

		explicit ErrorMessage(ErrorCode const& errorCode)
			: m_errorCode(errorCode)
		{
			header.messageType = MessageType::ErrorMessage;
			header.bodySize = GetBodySize();
		}

		bool ParseBody(std::vector<uint8_t> const& buffer) override
		{
			if (buffer.size() != GetBodySize())
				return false;

			std::memcpy(std::bit_cast<uint8_t*>(&m_errorCode), &buffer[0], sizeof(m_errorCode));

			return true;
		}

		std::vector<uint8_t> Serialize() const override
		{
			std::vector<uint8_t> buffer(MessageHeader::size + GetBodySize(), 0);
			std::vector<uint8_t> headerBuffer = header.Serialize();

			std::memcpy(&buffer[0], &headerBuffer[0], MessageHeader::size);
			std::memcpy(&buffer[MessageHeader::size], std::bit_cast<uint8_t*>(&m_errorCode), sizeof(m_errorCode));

			return buffer;
		}

		uint64_t GetBodySize() const final { return sizeof(m_errorCode); }

		[[nodiscard]] constexpr ErrorCode const& GetErrorCode() const { return m_errorCode; }
		constexpr void SetErrorCode(ErrorCode const& errorCode) { m_errorCode = errorCode; }

	private:
		ErrorCode m_errorCode;
	};
}