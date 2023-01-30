export module Server.Exception;

import <exception>;
import <string>;

export namespace Server
{
	struct Exception : std::exception
	{
		Exception(std::string_view const& message)
			: m_message{ message }
		{}

		char const* what() const noexcept override
		{
			return m_message.data();
		}

		std::string_view m_message;
	};
}