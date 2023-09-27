project(server_example)

file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

add_executable(
	example
	${HEADER_FILES}
	${SOURCE_FILES}
)

target_link_libraries(example PRIVATE shared)
target_link_libraries(example PRIVATE server)
target_link_libraries(example PRIVATE asio)
