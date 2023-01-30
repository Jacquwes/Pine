module;

#include <vector>

export module Socket.Messages:Hello;

import :Message;

export namespace Socket::Messages
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