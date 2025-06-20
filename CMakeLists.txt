cmake_minimum_required(VERSION 3.11)
project(lele)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0 # Or a later release
)
FetchContent_MakeAvailable(Catch2)

set(CMAKE_CXX_STANDARD 17)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
)
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/test/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# Remove main.cpp manually
list(REMOVE_ITEM TEST_FILES "${CMAKE_SOURCE_DIR}/src/main.cpp")

add_executable(engine ${SRC_FILES})
add_executable(test_engine ${TEST_FILES})

# Link Catch2 to your test executable
target_link_libraries(test_engine PRIVATE Catch2::Catch2WithMain)

target_include_directories(test_engine PRIVATE ${SRC_DIR})

