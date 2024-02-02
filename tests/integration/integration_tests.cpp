#include <semaphore>
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
	std::binary_semaphore server_ready(0);

	asio::io_context io_context;
	pine::server server(io_context);

	std::jthread server_thread([&]()
		{
			server.on_connection_failed(
				[&](
					pine::server& server,
					std::shared_ptr<pine::server_connection> const& client
					) -> async_task
				{
					ADD_FAILURE();
					server_ready.release();
					co_return;
				});

			server.on_connection(
				[&](
					pine::server& server,
					std::shared_ptr<pine::server_connection> const& client
					) -> async_task
				{
					server_ready.release();
					co_return;
				});

			server.on_ready(
				[&](pine::server& server) -> async_task
				{
					server_ready.release();
					co_return;
				});

			server.listen();
		});

	pine::client client("test", io_context);

	server_ready.acquire();

	if (!client.connect())
		ADD_FAILURE();

	server_ready.acquire();

	server.stop();

	server_thread.join();
}