cmake_minimum_required(VERSION 3.16)
project(hpalloc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -O3 -march=native)
endif()

add_library(hpalloc STATIC
        src/arena.cpp
        src/fixed_block.cpp
        src/size_class.cpp
)

target_include_directories(hpalloc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_executable(allocator_bench benchmarks/main.cpp)
target_link_libraries(allocator_bench hpalloc pthread)

add_executable(allocator_tests tests/main.cpp)
target_link_libraries(allocator_tests hpalloc pthread)


target_link_libraries(allocator_bench hpalloc pthread)
