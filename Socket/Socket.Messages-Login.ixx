export module Socket.Messages:Login;

import <cstdint>;
import <vector>;
import :Message;

export namespace Socket::Messages
{
	struct Login : Message
	{
		Login() { header.type = MessageType::Login; }

		static size_t const size = 0;

		bool ParseBody(std::vector<uint8_t> const& buffer) const
		{
			return false;
		}
	};
}