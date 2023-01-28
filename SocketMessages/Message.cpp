#include "pch.h"

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

	bool Message::ParseBody(std::vector<uint8_t> const& buffer)
	{
		return false;
	}
}