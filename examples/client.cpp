#include <iostream>

#include <asio.hpp>
#include <client_connection.h>

int main()
{
	asio::io_context context;
	asio::ip::tcp::socket socket(context);
	pine::client_connection client(socket);

	client.connect("localhost");

	while (true);
}