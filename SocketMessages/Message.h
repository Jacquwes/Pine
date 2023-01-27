#pragma once

#include "pch.h"

namespace SocketMessages
{
	enum class MessageType : uint8_t
	{
		Invalid,
		Hello
	};

	struct Message
	{
		Message(MessageType const& type, size_t const& size)
			: type{ type }
			, size{ size }
		{
		}
		
		virtual ~Message() = default;
		virtual bool Serialize() { return false; }

		MessageType type;
		size_t size;
	};
}