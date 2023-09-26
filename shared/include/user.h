#pragma once

#include <memory>
#include <string>

namespace pine
{
	/// @brief A user of the chat application.
	class user : public std::enable_shared_from_this<user>
	{
	public:
		/// @brief Construct a user with an empty username.
		user() = default;

		/// @brief Construct a user with the given username.
		user(std::string username);

		/// @brief Destroy the user.
		~user() = default;

		/// @brief Username of the user.
		std::string username{ "" };
	};
}
