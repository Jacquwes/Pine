#pragma once

#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"

namespace SocketMessages
{
	struct ReceiveChatMessage : Message
	{
		ReceiveChatMessage()
		{
			header.messageType = MessageType::ReceiveChatMessage;
			header.bodySize = GetBodySize();
		}

		bool ParseBody(std::vector<uint8_t> const& buffer) override
		{
			if (buffer.size() != GetBodySize())
				return false;

			std::memcpy(
				std::bit_cast<uint8_t*>(&m_authorUsernameLength),
				buffer.data(),
				sizeof(m_authorUsernameLength)
			);
			m_authorUsername = std::string(
				buffer.begin() + sizeof(m_authorUsernameLength),
				buffer.begin() + sizeof(m_authorUsernameLength) + m_authorUsernameLength
			);

			std::memcpy(
				std::bit_cast<uint16_t*>(&m_chatMessageLength),
				buffer.data() + sizeof(m_authorUsernameLength) + m_authorUsernameLength,
				sizeof(m_chatMessageLength)
			);
			m_chatMessage = std::string(
				buffer.begin() + sizeof(m_authorUsernameLength) + m_authorUsernameLength + sizeof(m_chatMessageLength),
				buffer.end()
			);

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
			std::memcpy(&buffer[cursor], &m_authorUsernameLength, sizeof(m_authorUsernameLength));
			cursor += sizeof(m_authorUsernameLength);
			std::memcpy(&buffer[cursor], &m_authorUsername[0], m_authorUsernameLength);
			cursor += m_authorUsernameLength;
			std::memcpy(&buffer[cursor], &m_chatMessageLength, sizeof(m_chatMessageLength));
			cursor += sizeof(m_chatMessageLength);
			std::memcpy(&buffer[cursor], &m_chatMessage[0], m_chatMessageLength);

			return buffer;
		}

		uint64_t GetBodySize() const final 
		{
			return sizeof(m_authorUsernameLength)
				+ m_authorUsernameLength
				+ sizeof(m_chatMessageLength)
				+ m_chatMessageLength;
		}

		[[nodiscard]] constexpr std::string const& GetAuthorUsername() const { return m_authorUsername; }
		constexpr bool SetAuthorUsername(std::string_view const& authorUsername)
		{
			if (authorUsername.length() < UsernameMinLength || authorUsername.length() > UsernameMaxLength)
				return false;

			m_authorUsername = authorUsername;
			m_authorUsernameLength = static_cast<uint8_t>(authorUsername.length());

			header.bodySize = GetBodySize();

			return true;
		}

		[[nodiscard]] constexpr std::string const& GetChatMessage() const { return m_chatMessage; }
		constexpr bool SetChatMessage(std::string_view const& chatMessage)
		{
			if (chatMessage.length() < ChatMessageMinLength || chatMessage.length() > ChatMessageMaxLength)
				return false;

			m_chatMessage = chatMessage;
			m_chatMessageLength = static_cast<uint16_t>(chatMessage.length());

			header.bodySize = GetBodySize();

			return true;
		}

	private:
		uint8_t m_authorUsernameLength{};
		std::string m_authorUsername{};
		uint16_t m_chatMessageLength{};
		std::string m_chatMessage{};
	};
}