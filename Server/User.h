#pragma once

#include <memory>
#include <string>

class User : public std::enable_shared_from_this<User>
{
public:
	User() = default;
	~User() = default;

	[[nodiscard]] constexpr std::string const& GetUsername() const noexcept { return m_username; }
	constexpr void SetUsername(std::string_view const& username) { m_username = username; }

private:
	std::string m_username{ "" };
};
