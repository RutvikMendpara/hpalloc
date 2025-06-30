#pragma once
#include <cstddef>


class Arena {
public:
    explicit Arena(size_t size_in_bytes);
    void* alloc_page();
    size_t pages_allocated() const;
    size_t total_pages() const;
    ~Arena();

private:
    void* base_;
    size_t total_size_;
    size_t page_size_;
    size_t total_pages_;
    size_t next_page_index_;
};
