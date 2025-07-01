#include "hpalloc/allocator.h"
#include <gtest/gtest.h>
#include <unordered_set>

TEST(SizeClassAllocatorTest, AllocateAndDeallocateAllClasses) {
    SizeClassAllocator allocator;
    static const size_t kTestSizes[] = {1, 16, 32, 33, 64, 100, 128, 200, 256};

    for (size_t request_size : kTestSizes) {
        size_t class_size = 0;
        void* ptr = allocator.allocate(request_size, class_size);

        ASSERT_NE(ptr, nullptr);
        ASSERT_GE(class_size, request_size);
        ASSERT_TRUE(class_size == 32 || class_size == 64 ||
                    class_size == 128 || class_size == 256);
        ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr) % class_size, 0);  // Alignment

        allocator.deallocate(ptr, class_size);
    }
}

TEST(SizeClassAllocatorTest, RejectOversizedAllocation) {
    SizeClassAllocator allocator;
    size_t class_size = 0;

    EXPECT_THROW({
        allocator.allocate(512, class_size);
    }, std::invalid_argument);
}

TEST(SizeClassAllocatorTest, UniqueAllocations) {
    SizeClassAllocator allocator;
    std::unordered_set<void*> seen;
    size_t class_size;

    for (int i = 0; i < 1000; ++i) {
        void* ptr = allocator.allocate(64, class_size);
        ASSERT_NE(ptr, nullptr);
        ASSERT_TRUE(seen.insert(ptr).second); // Ensure no duplicates
    }
}

TEST(SizeClassAllocatorTest, DoubleFreeIsSafeFromUserSide) {
    SizeClassAllocator allocator;
    size_t class_size;
    void* ptr = allocator.allocate(64, class_size);
    allocator.deallocate(ptr, class_size);

    // If user double-frees with same size class, allocator won't detect
    // but allocator itself won’t crash unless corrupted
    allocator.deallocate(ptr, class_size);
}

TEST(SizeClassAllocatorTest, InvalidDeallocationClassThrows) {
    SizeClassAllocator allocator;
    size_t class_size;
    void* ptr = allocator.allocate(64, class_size);

    EXPECT_THROW({
        allocator.deallocate(ptr, 999); // Invalid class
    }, std::invalid_argument);

    allocator.deallocate(ptr, class_size); // Proper cleanup
}
