project(shared)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB HEADER_FILES "include/*.h")
file(GLOB SOURCE_FILES "src/*.cpp")

add_library(shared STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(shared PUBLIC include)