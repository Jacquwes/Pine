#pragma once

#include "Message.h"

namespace SocketMessages
{
	struct Hello : Message
	{
		Hello() { header.type = MessageType::Hello; }
		
		static size_t const size = 0;
		
		std::vector<uint8_t> Serialize() const override
		{
			return header.Serialize();
		}
	};
}