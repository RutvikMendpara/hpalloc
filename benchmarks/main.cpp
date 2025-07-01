#include <chrono>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "hpalloc/allocator.h"

// size class benchmark
void run_size_class_benchmark();

// standalone benchmark runner
using Clock = std::chrono::high_resolution_clock;
void* allocate_block(SizeClassAllocator& alloc, size_t size, size_t& class_size) {
    return alloc.allocate(size, class_size);
}

int main(int argc, char* argv[]) {
    std::string allocator_type = "fastalloc";
    size_t block_size = 64;
    size_t count = 10000000;

    // CLI parsing
    for (int i = 1; i < argc; ++i) {
        if (std::strncmp(argv[i], "--allocator=", 12) == 0) {
            allocator_type = std::string(argv[i] + 12);
        } else if (std::strncmp(argv[i], "--block-size=", 13) == 0) {
            block_size = std::stoul(argv[i] + 13);
        } else if (std::strncmp(argv[i], "--count=", 8) == 0) {
            count = std::stoul(argv[i] + 8);
        }
    }

    std::cout << "[Config] Allocator: " << allocator_type
              << ", Block Size: " << block_size
              << ", Count: " << count << "\n";

    std::vector<void*> ptrs(count);
    std::vector<double> durations;
    durations.reserve(count);

    auto start = Clock::now();

    if (allocator_type == "fastalloc") {
        SizeClassAllocator alloc;
        size_t class_size = 0;
        for (size_t i = 0; i < count; ++i) {
            auto t0 = Clock::now();
            ptrs[i] = allocate_block(alloc, block_size, class_size);
            auto t1 = Clock::now();
            durations.push_back(std::chrono::duration<double, std::nano>(t1 - t0).count());
        }
        for (size_t i = 0; i < count; ++i) {
            alloc.deallocate(ptrs[i], class_size);
        }
    } else if (allocator_type == "malloc") {
        for (size_t i = 0; i < count; ++i) {
            auto t0 = Clock::now();
            ptrs[i] = std::malloc(block_size);
            auto t1 = Clock::now();
            durations.push_back(std::chrono::duration<double, std::nano>(t1 - t0).count());
        }
        for (size_t i = 0; i < count; ++i) {
            std::free(ptrs[i]);
        }
    } else {
        std::cerr << "Unknown allocator type: " << allocator_type << "\n";
        return 1;
    }

    auto end = Clock::now();
    double total_ns = std::chrono::duration<double, std::nano>(end - start).count();
    double avg_ns = total_ns / count;

    std::sort(durations.begin(), durations.end());
    double p99 = durations[size_t(count * 0.99)];

    std::cout << "[Perf] Total time: " << total_ns / 1e6 << " ms\n";
    std::cout << "[Perf] Avg latency: " << avg_ns << " ns\n";
    std::cout << "[Perf] P99 latency: " << p99 << " ns\n";
    std::cout << "[Perf] RSS: [Not Implemented]\n";
    return 0;
}

void run_size_class_benchmark() {
    constexpr size_t N = 10'000'000;

    for (size_t size : kSizeClasses) {
        SizeClassAllocator allocator;
        std::vector<void*> blocks;
        blocks.reserve(N);

        size_t actual_class;
        auto start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < N; ++i)
            blocks.push_back(allocator.allocate(size, actual_class));

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;

        std::cout << "[Perf] Allocated " << N << " blocks of size " << size
                  << " in " << duration.count() << " sec\n";

        for (void* ptr : blocks)
            allocator.deallocate(ptr, actual_class);
    }
}
