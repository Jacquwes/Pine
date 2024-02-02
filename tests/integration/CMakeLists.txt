include_directories()
include(GoogleTest)

file (GLOB_RECURSE SOURCES *.cpp)

add_executable(
	integration_tests
	${SOURCES}
)

target_link_libraries(integration_tests PRIVATE asio)
target_link_libraries(integration_tests PRIVATE client)
target_link_libraries(integration_tests PRIVATE server)
target_link_libraries(integration_tests PRIVATE shared)
target_link_libraries(integration_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

gtest_discover_tests(integration_tests)