
#include <asio.hpp>
#include <coroutine.h>

#include "client_connection.h"

namespace pine
{
	client_connection::client_connection(asio::ip::tcp::socket& client_socket)
		: connection{ client_socket }
	{}

	bool client_connection::connect(std::string const& host, uint16_t const& port)
	{
		asio::ip::tcp::resolver::query resolver_query(
			host,
			std::to_string(port),
			asio::ip::tcp::resolver::query::numeric_service
		);

		asio::ip::tcp::resolver resolver{ socket.get_executor() };

		asio::ip::tcp::resolver::iterator it =
			resolver.resolve(resolver_query, ec);

		if (ec)
			return false;

		asio::connect(socket, it, ec);

		if (ec)
			return false;

		return true;
	}
}