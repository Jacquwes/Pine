#include <cstdint>
#include <memory>
#include <vector>

#include "Message.h"
#include "Hello.h"

namespace SocketMessages
{
	MessageHeader::MessageHeader(std::vector<uint8_t> const& buffer)
	{
		Parse(buffer);
	}

	void MessageHeader::Parse(std::vector<uint8_t> const& buffer)
	{
		size = 3;
		std::memcpy(&size, &buffer[1], sizeof(size));

		switch (uint8_t messageType = buffer[0])
		{
			using enum SocketMessages::MessageType;
		case static_cast<int>(Hello):
			type = Hello;
			if (size != SocketMessages::Hello::size)
			{
				type = Invalid;
			}
			break;
		case static_cast<int>(Login):
			type = Login;
			break;
		default:
			type = Invalid;
			break;
		}
	}

	std::vector<uint8_t> MessageHeader::Serialize() const
	{
		std::vector<uint8_t> buffer;
		buffer.push_back(static_cast<uint8_t>(type));
		buffer.resize(9);
		std::memcpy(&buffer[1], &size, sizeof(size));
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