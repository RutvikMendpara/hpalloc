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
# GoogleTest setup
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
# Test target
# --------------------
add_executable(hpalloc_tests
        tests/arena_test.cpp
        tests/fixed_block_test.cpp
        tests/main.cpp
)
target_link_libraries(hpalloc_tests
        hpalloc
        gtest
        gtest_main
        pthread
)
add_test(NAME AllTests COMMAND hpalloc_tests)

# --------------------
# Benchmark target (optional)
# --------------------
add_executable(allocator_bench benchmarks/main.cpp)
target_link_libraries(allocator_bench hpalloc pthread)
