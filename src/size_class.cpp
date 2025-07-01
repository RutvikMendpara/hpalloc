#include "hpalloc/size_class.h"

SizeClassAllocator::SizeClassAllocator(size_t arena_size_per_class) {
    for (size_t i = 0; i < kNumClasses; ++i) {
        size_t size_class = kSizeClasses[i];
        auto arena = std::make_unique<Arena>(arena_size_per_class);

        allocators_.emplace(size_class, Fixed_Block(*arena, size_class));
        arenas_[i] = std::move(arena);
    }
}

void* SizeClassAllocator::allocate(size_t size, size_t& actual_class_size) {
    for (size_t sc : kSizeClasses) {
        if (size <= sc) {
            actual_class_size = sc;
            return allocators_.at(sc).allocate();
        }
    }
    throw std::invalid_argument("Unsupported allocation size");
}

void SizeClassAllocator::deallocate(void* ptr, size_t actual_class_size) {
    auto it = allocators_.find(actual_class_size);
    if (it == allocators_.end()) {
        throw std::invalid_argument("Invalid size class passed to deallocate");
    }
    it->second.deallocate(ptr);
}

