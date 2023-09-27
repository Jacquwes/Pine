// Purpose: Source file for server example.

#include <iostream>

#include <asio.hpp>
#include <server.h>

int main()
{
	asio::io_context context{};
	pine::server server{ context };

	server.listen();
}