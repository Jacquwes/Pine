#include <thread>

#include <asio.hpp>
#include <client.h>
#include <coroutine.h>
#include <gtest/gtest.h>
#include <server.h>

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(integration_tests, connect_client_to_server)
{
	asio::io_context io_context;

	std::jthread server_thread([&]()
		{
			pine::server server(io_context);

			server.on_connection_attempt([](auto& server, auto& client) -> async_task
				{
					server.stop();
					SUCCEED();
					co_return;
				});

			server.listen();

			FAIL();
		});

	pine::client client("test", io_context);

	if(!client.connect())
		FAIL();

	client.disconnect();
}