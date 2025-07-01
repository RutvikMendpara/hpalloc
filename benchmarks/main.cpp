#include <chrono>
#include <iostream>
#include <vector>
#include "hpalloc/allocator.h"

void run_size_class_benchmark();
int main() {
    run_size_class_benchmark();
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
