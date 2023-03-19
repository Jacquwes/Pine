#include <cstdint>
#include <memory>
#include <vector>

#include "Message.h"
#include "HelloMessage.h"

namespace SocketMessages
{
	MessageHeader::MessageHeader(std::vector<uint8_t> const& buffer)
	{
		Parse(buffer);
	}

	void MessageHeader::Parse(std::vector<uint8_t> const& buffer)
	{
		std::memcpy(&bodySize, &buffer[1], sizeof(bodySize));

		switch (uint8_t messageType_ = buffer[0])
		{
			using enum SocketMessages::MessageType;
		case static_cast<int>(HelloMessage):
			messageType = HelloMessage;
			if (bodySize != SocketMessages::HelloMessage::size)
			{
				messageType = InvalidMessage;
			}
			break;
		case static_cast<int>(LoginMessage):
			messageType = LoginMessage;
			break;
		default:
			messageType = InvalidMessage;
			break;
		}
	}

	std::vector<uint8_t> MessageHeader::Serialize() const
	{
		std::vector<uint8_t> buffer;
		buffer.push_back(static_cast<uint8_t>(messageType));
		buffer.resize(9);
		std::memcpy(&buffer[1], &bodySize, sizeof(size));
		return buffer;
	}

	bool Message::ParseBody(std::vector<uint8_t> const& buffer)
	{
		return false;
	}

	std::vector<uint8_t> Message::Serialize() const
	{
		std::vector<uint8_t> buffer(9, 0);
		return buffer;
	}
}