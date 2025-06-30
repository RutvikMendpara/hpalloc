#pragma once

#include "hpalloc/arena.h"
#include <cstddef>

class Fixed_Block {
public:
    Fixed_Block(Arena& arena, size_t block_size = 64);
    void* allocate();
    void deallocate(void* ptr);
private:
    void refill();  // Get a new page from arena and chop into blocks
    struct FreeBlock {
        FreeBlock* next;
    };

    FreeBlock* free_list_;
    Arena& arena_;
    size_t block_size_;
};