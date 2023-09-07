include_directories()

add_executable(
	unit_tests
	snowflake_tests.cpp
)

target_link_libraries(unit_tests PRIVATE shared)
target_link_libraries(unit_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(
	NAME
		UnitTests
	COMMAND
		unit_tests
)