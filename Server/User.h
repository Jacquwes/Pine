#pragma once

#include <memory>
#include <string>

class Connection;

class User : public std::enable_shared_from_this<User>
{
public:
	User() = default;
	~User() = default;

	[[nodiscard]] constexpr std::string const& GetUsername() const noexcept { return m_username; }
	[[nodiscard]] constexpr bool const& IsLoggedIn() const noexcept { return m_isLoggedIn; }

private:
	bool m_isLoggedIn{ false };
	std::string m_username{ "" };

	friend class Connection;
};
