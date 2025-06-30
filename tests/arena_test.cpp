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