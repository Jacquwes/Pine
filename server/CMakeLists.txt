project(server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE HEADER_FILES "include/*.h")
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

add_library(server STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(server PUBLIC include)

target_link_libraries(server PRIVATE shared)
