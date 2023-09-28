#include <cstdint>
#include <string>

#include <asio.hpp>
#include <connection.h>

#include "client_connection.h"
#include "client.h"

namespace pine
{
	client_connection::client_connection(client& client, asio::io_context& io_context)
		: connection{ asio::ip::tcp::socket{ io_context } },
		client_ref{ client }
	{}

	bool client_connection::connect(std::string const& host, uint16_t const& port)
	{
		asio::ip::tcp::resolver::query resolver_query(
			host,
			std::to_string(port),
			asio::ip::tcp::resolver::query::numeric_service
		);

		asio::ip::tcp::resolver resolver{ client_ref.io_context };

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