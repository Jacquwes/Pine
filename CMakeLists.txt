cmake_minimum_required(VERSION 3.15)

project(Pine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(GTest CONFIG REQUIRED)

add_subdirectory(shared)
add_subdirectory(tests)