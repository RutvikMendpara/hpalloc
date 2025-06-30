#include "hpalloc/allocator.h"
#include <gtest/gtest.h>
#include <unordered_set>
#include <vector>

constexpr size_t kPageSize = 4096;
constexpr size_t kBlockSize = 64;

TEST(FixedBlockTest, AllocAndDeallocSingleBlock) {
    Arena arena(kPageSize);
    Fixed_Block allocator(arena, kBlockSize);

    void* block = allocator.allocate();
    ASSERT_NE(block, nullptr);
    allocator.deallocate(block);
}


TEST(FixedBlockTest, AllocateMultipleBlocks) {
    Arena arena(4 * kPageSize);  // Enough for 256 blocks
    Fixed_Block allocator(arena, kBlockSize);

    std::unordered_set<void*> blocks;
    for (int i = 0; i < 200; ++i) {
        void* ptr = allocator.allocate();
        ASSERT_NE(ptr, nullptr);
        ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr) % kBlockSize, 0);
        ASSERT_TRUE(blocks.insert(ptr).second);  // ensure uniqueness
    }
}

TEST(FixedBlockTest, ExhaustAllocatorReturnsNull) {
    Arena arena(kPageSize);  // ~64 blocks
    Fixed_Block allocator(arena, kBlockSize);

    for (size_t i = 0; i < kPageSize / kBlockSize; ++i) {
        ASSERT_NE(allocator.allocate(), nullptr);
    }

    // Next allocation should fail (arena exhausted)
    ASSERT_EQ(allocator.allocate(), nullptr);
}

TEST(FixedBlockTest, ReuseDeallocatedBlock) {
    Arena arena(kPageSize);
    Fixed_Block allocator(arena, kBlockSize);

    void* a = allocator.allocate();
    allocator.deallocate(a);
    void* b = allocator.allocate();

    // 'b' should be equal to 'a' (LIFO free list)
    ASSERT_EQ(b, a);
}