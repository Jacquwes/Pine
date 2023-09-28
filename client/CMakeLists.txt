project(client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES "include/*.h")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

add_library(client STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(client PUBLIC include)

target_link_libraries(client PRIVATE shared)
target_link_libraries(client PRIVATE asio)
