#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"
#include "User.h"

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
			if (buffer.size() < sizeof(m_chatMessageLength) + ChatMessageMinLength
				|| buffer.size() > sizeof (m_chatMessageLength) + ChatMessageMaxLength)
				return false;

			std::memcpy(std::bit_cast<uint8_t*>(&m_chatMessageLength),
						buffer.data(), sizeof(m_chatMessageLength));
			m_chatMessage = std::string(buffer.begin() + sizeof(m_chatMessageLength),
										buffer.begin() + sizeof(m_chatMessageLength) + m_chatMessageLength);

			header.bodySize = GetBodySize();

			return true;
		}

		std::vector<uint8_t> Serialize() const override
		{
			std::vector<uint8_t> buffer(MessageHeader::size + GetBodySize(), 0);
			std::vector<uint8_t> headerBuffer = header.Serialize();

			uint8_t cursor = 0;

			std::memcpy(&buffer[cursor], &headerBuffer[0], MessageHeader::size);
			cursor += MessageHeader::size;
			std::memcpy(&buffer[cursor], &m_chatMessageLength, sizeof(m_chatMessageLength));
			cursor += sizeof(m_chatMessageLength);
			std::memcpy(&buffer[cursor], &m_chatMessage[0], m_chatMessageLength);

			return buffer;
		}

		uint64_t constexpr GetBodySize() const final
		{
			return sizeof(m_chatMessageLength) + m_chatMessageLength;
		}

		[[nodiscard]] constexpr std::string const& GetChatMessage() const { return m_chatMessage; }
		constexpr bool SetChatMessage(std::string_view const& chatMessage)
		{
			if (chatMessage.size() > ChatMessageMaxLength || chatMessage.size() < ChatMessageMinLength)
				return false;

			m_chatMessage = chatMessage;
			m_chatMessageLength = static_cast<uint8_t>(chatMessage.size());

			header.bodySize = GetBodySize();

			return true;
		}

	private:
		uint16_t m_chatMessageLength{};
		std::string m_chatMessage{};
	};
}