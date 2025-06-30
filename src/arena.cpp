#include "hpalloc/arena.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>

constexpr size_t MAX_ALLOWED_BYTES = 256 * 1024 * 1024;

Arena::Arena(size_t size_in_bytes) {
    page_size_ = static_cast<size_t>(getpagesize());
    total_size_ = size_in_bytes;

    if (size_in_bytes == 0 || size_in_bytes > MAX_ALLOWED_BYTES || size_in_bytes % page_size_ != 0) {
        throw std::invalid_argument("Arena size must be >0, <=256MB, and page aligned");
    }

    total_pages_ = total_size_ / page_size_;
    next_page_index_ = 0;

    base_ = mmap(nullptr, total_size_,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (base_ == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }
}

void* Arena::alloc_page() {
    if (next_page_index_ >= total_pages_) {
        return nullptr; //OOM
    }

    void* addr = static_cast<char*>(base_) + (next_page_index_ * page_size_);
    ++next_page_index_;
    return addr;
}

size_t Arena::pages_allocated() const {
    return next_page_index_;
}

size_t Arena::total_pages() const {
    return total_pages_;
}
size_t Arena::page_size() const {
    return page_size_;
}

Arena::~Arena() {
    if (base_) {
        munmap(base_, total_size_);
    }
}
