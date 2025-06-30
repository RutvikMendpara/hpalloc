#include "hpalloc/allocator.h"
#include <gtest/gtest.h>

TEST(ArenaTest, AllocatesSinglePage) {
    Arena arena(4 * 4096);
    void* p = arena.alloc_page();
    ASSERT_NE(p, nullptr);
    ASSERT_EQ(arena.pages_allocated(), 1);
}

TEST(ArenaTest, ThrowsOnInvalidSize) {
    EXPECT_THROW(Arena(0),std::invalid_argument);
    EXPECT_THROW(Arena(123),std::invalid_argument); // not page aligned
    EXPECT_THROW(Arena(512 * 1024 * 1024),std::invalid_argument); // >256
}

TEST(ArenaTest, ExhaustsPages) {
    Arena arena(2 * 4096);
    void* p1 = arena.alloc_page();
    void* p2 = arena.alloc_page();
    void* p3 = arena.alloc_page();
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    ASSERT_EQ(p3, nullptr);
    ASSERT_EQ(arena.pages_allocated(), 2);
}

TEST(ArenaTest, TracksPagesCorrectly) {
    Arena arena(10 * 4096);

    ASSERT_EQ(arena.total_pages(), 10);

    ASSERT_NE(arena.alloc_page(), nullptr);

    for (int i = 1; i < 10; ++i) {
        ASSERT_NE(arena.alloc_page(), nullptr);
        ASSERT_EQ(arena.pages_allocated(), i + 1);
    }

    ASSERT_EQ(arena.alloc_page(), nullptr);
    ASSERT_EQ(arena.pages_allocated(), 10);
}


TEST(ArenaTest, AllocatedPagesAreAlignedAndUnique) {
    constexpr size_t page_count = 100;
    Arena arena(page_count * 4096);

    std::vector<void*> addrs;

    for (size_t i = 0; i < page_count; ++i) {
        void* p = arena.alloc_page();
        ASSERT_NE(p, nullptr);
        ASSERT_EQ(reinterpret_cast<uintptr_t>(p) % 4096, 0); // 4KB aligned
        addrs.push_back(p);
    }

    // Check uniqueness
    std::sort(addrs.begin(), addrs.end());
    auto it = std::unique(addrs.begin(), addrs.end());
    ASSERT_TRUE(it == addrs.end()); // no duplicates
}
