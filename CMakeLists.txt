cmake_minimum_required(VERSION 3.16)
project(hpalloc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -O3 -march=native)
endif()

# --------------------
# Allocator library
# --------------------
add_library(hpalloc STATIC
        src/arena.cpp
        src/fixed_block.cpp
        src/size_class.cpp
)

target_include_directories(hpalloc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# --------------------
# GoogleTest (before test targets)
# --------------------
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

enable_testing()

# --------------------
# Benchmark target
# --------------------
add_executable(allocator_bench benchmarks/main.cpp)
target_link_libraries(allocator_bench hpalloc pthread)

# --------------------
# Test target
# --------------------
add_executable(allocator_tests
        tests/main.cpp
        tests/arena_test.cpp
)

target_link_libraries(allocator_tests
        hpalloc
        gtest
        gtest_main
        pthread
)
