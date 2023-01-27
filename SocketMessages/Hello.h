#pragma once

#include "Message.h"

namespace SocketMessages
{
	struct Hello : Message
	{
		Hello()
			: Message{ MessageType::Hello, 0 }
		{}
	};
}