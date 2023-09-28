project(examples)

add_executable(
	server_example
	server.cpp
)

target_link_libraries(server_example PRIVATE shared)
target_link_libraries(server_example PRIVATE server)
target_link_libraries(server_example PRIVATE asio)
