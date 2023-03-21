#pragma once

#include <bit>
#include <cstdint>
#include <vector>

#include "Message.h"

constexpr uint64_t CurrentVersion = 0x2;

namespace SocketMessages
{
	struct HelloMessage : Message
	{
		HelloMessage()
		{
			header.messageType = MessageType::HelloMessage;
			header.bodySize = GetBodySize();
		}

		bool ParseBody(std::vector<uint8_t> const& buffer) override
		{
			if (buffer.size() != GetBodySize())
				return false;

			std::memcpy(std::bit_cast<void*>(&m_version), std::bit_cast<void*>(buffer.data()), sizeof(m_version));
			return true;
		}

		std::vector<uint8_t> Serialize() const override
		{
			std::vector<uint8_t> buffer(MessageHeader::size + GetBodySize(), 0);
			std::vector<uint8_t> headerBuffer = header.Serialize();

			std::memcpy(&buffer[0], &headerBuffer[0], MessageHeader::size);
			std::memcpy(&buffer[MessageHeader::size], &m_version, sizeof(m_version));

			return buffer;
		}

		uint64_t GetBodySize() const final { return sizeof(m_version); }

		constexpr uint64_t const& GetVersion() const { return m_version; }
		constexpr void SetVersion(uint64_t const& version) { m_version = version; }

	private:
		uint64_t m_version{ CurrentVersion };
	};
}