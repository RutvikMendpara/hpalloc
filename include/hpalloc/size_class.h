#pragma once


#include "hpalloc/fixed_block.h"
#include "hpalloc/arena.h"

#include <array>
#include <cstddef>
#include <memory>
#include <unordered_map>

inline constexpr size_t kSizeClasses[] = {32, 64, 128, 256};
inline constexpr size_t kNumClasses = std::size(kSizeClasses);

class SizeClassAllocator {
public:
    explicit SizeClassAllocator(size_t arena_size_per_class = 1 << 20); // 1MB by default
    void* allocate(size_t size, size_t& actual_class_size);
    void deallocate(void* ptr, size_t actual_class_size);

private:
    std::unordered_map<size_t, Fixed_Block> allocators_;
    std::array<std::unique_ptr<Arena>, kNumClasses> arenas_;

};

