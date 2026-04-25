cmake_minimum_required(VERSION 3.11)
project(lele)

include(FetchContent)

# --------------------------
# Catch2 for testing
# --------------------------
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
)
FetchContent_MakeAvailable(Catch2)

# --------------------------
# C++ standard
# --------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_DEBUG   "-g -O0")
# --------------------------
# Source directories
# --------------------------
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.hpp"
)

file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS
    "${TEST_DIR}/*.cpp"
    "${SRC_DIR}/*.cpp"
)

# Remove main.cpp from test files
list(REMOVE_ITEM TEST_FILES "${SRC_DIR}/main.cpp")

# --------------------------
# Option: enable gperftools profiling
# --------------------------
option(ENABLE_GPERFTOOLS "Enable Google PerfTools CPU profiling" OFF)

# --------------------------
# Executables
# --------------------------
add_executable(engine ${SRC_FILES})
add_executable(test_engine ${TEST_FILES})

# --------------------------
# Link Catch2 for test_engine
# --------------------------
target_link_libraries(test_engine PRIVATE Catch2::Catch2WithMain)
target_include_directories(test_engine PRIVATE ${SRC_DIR})

# Define a default value if the user doesn't provide one
set(ENABLE_PERFT 1 CACHE STRING "Enable perft")

# Pass it to the test executable
target_compile_definitions(test_engine PRIVATE ENABLE_PERFT=${ENABLE_PERFT})
# --------------------------
# Link gperftools to engine if enabled
# --------------------------
if(ENABLE_GPERFTOOLS)
    # Detect Homebrew paths
    if(APPLE AND EXISTS "/opt/homebrew/include/gperftools/profiler.h")
        set(PROFILER_INCLUDE_DIR "/opt/homebrew/include")
        set(PROFILER_LIB "/opt/homebrew/lib/libprofiler.dylib")
    elseif(EXISTS "/usr/local/include/gperftools/profiler.h")
        set(PROFILER_INCLUDE_DIR "/usr/local/include")
        set(PROFILER_LIB "/usr/local/lib/libprofiler.dylib")
    else()
        message(FATAL_ERROR "gperftools not found. Make sure it is installed via Homebrew.")
    endif()

    message(STATUS "gperftools include: ${PROFILER_INCLUDE_DIR}")
    message(STATUS "gperftools lib: ${PROFILER_LIB}")

    target_include_directories(engine PRIVATE ${PROFILER_INCLUDE_DIR})
    target_link_libraries(engine PRIVATE ${PROFILER_LIB})
    target_compile_definitions(engine PRIVATE ENABLE_PROFILER)
endif()
