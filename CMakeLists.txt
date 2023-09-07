cmake_minimum_required(VERSION 3.15)

project(Pine)

enable_testing()

find_package(GTest CONFIG REQUIRED)

add_subdirectory(shared)
add_subdirectory(tests)