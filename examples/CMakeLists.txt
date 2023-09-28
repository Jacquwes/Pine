project(examples)

add_executable(
	server_example
	server.cpp
)

target_link_libraries(server_example PRIVATE shared)
target_link_libraries(server_example PRIVATE server)
target_link_libraries(server_example PRIVATE asio)

add_executable(
	client_example
	client.cpp
)

target_link_libraries(client_example PRIVATE shared)
target_link_libraries(client_example PRIVATE client)
target_link_libraries(client_example PRIVATE asio)
