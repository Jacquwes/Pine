#include <iostream>

#include <asio.hpp>
#include <client.h>

int main()
{
	asio::io_context context;
	pine::client client("Example", context);

	if (client.connect("localhost"))
	{
		std::cout << "Connected to server" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect to server" << std::endl;
		return 1;
	}
}