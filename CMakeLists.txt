cmake_minimum_required(VERSION 3.15)

project(Pine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(asio CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

if (WIN32)
	add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

add_subdirectory(client)
add_subdirectory(examples)
add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(tests)