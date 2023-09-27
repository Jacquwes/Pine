include_directories()
include(GoogleTest)

file (GLOB_RECURSE SOURCES *.cpp)

add_executable(
	unit_tests
	${SOURCES}
)

target_link_libraries(unit_tests PRIVATE shared)
target_link_libraries(unit_tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

gtest_discover_tests(unit_tests)