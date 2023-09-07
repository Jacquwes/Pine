#pragma once

#include <memory>
#include <string>

namespace pine
{
	class user : public std::enable_shared_from_this<user>
	{
	public:
		user(std::string username);
		~user() = default;

		const std::string username{ "" };
	};
}
