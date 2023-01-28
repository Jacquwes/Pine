#pragma once

#include "pch.h"

#include "Message.h"

namespace SocketMessages
{
	struct Hello : Message
	{
		Hello() = default;
		static size_t const size = 0;
	};
}