#pragma once
#include <cstddef>

class Area {
public:
    explicit Area(size_t size_in_bytes);
    void* alloc_page();
    size_t page_allocated() const;
    size_t total_pages() const;
    ~Area();

private:
    void* base_;
    size_t total_size_;
    size_t page_size_;
    size_t total_pages_;
    size_t next_page_index_;
};
