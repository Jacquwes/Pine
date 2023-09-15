include_directories()

file (GLOB_RECURSE SOURCES *.cpp)

add_executable(
	integration_tests
	${SOURCES}
)

target_link_libraries(integration_tests PRIVATE shared)
target_link_libraries(integration_tests PRIVATE server)
target_link_libraries(integration_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(
	NAME
		IntegrationTests
	COMMAND
		integration_tests
)