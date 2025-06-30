#include "hpalloc/fixed_block.h"
#include <stdexcept>

Fixed_Block::Fixed_Block(Arena& arena, size_t block_size):
    free_list_(nullptr), arena_(arena), block_size_(block_size) {
    if (block_size_ < sizeof(FreeBlock*)) {
        throw std::invalid_argument("Block size too small for freelist pointer");
    }

    if (block_size_ % alignof(std::max_align_t) != 0) {
        throw std::invalid_argument("Block size must be aligned");
    }

    refill();
}
void Fixed_Block::refill() {
    void* page = arena_.alloc_page();
    if (!page) return; // Arena exhausted

    char* block = static_cast<char *>(page);
    size_t blocks_per_page = arena_.page_size() / block_size_;

    for (size_t i = 0; i < blocks_per_page; ++i) {
        FreeBlock* fb = reinterpret_cast<FreeBlock*>(block);
        fb->next = free_list_;
        free_list_ = fb;
        block += block_size_;
    }
}

void* Fixed_Block::allocate() {
    if (!free_list_) {
        refill();
        if (!free_list_) {
            return nullptr; // arena exhausted
        }
    }

    void * block = free_list_;
    free_list_ = free_list_->next;
    return block;
}

void Fixed_Block::deallocate(void* ptr) {
    if (!ptr) return;
    auto* block = static_cast<FreeBlock*>(ptr);
    block->next = free_list_;
    free_list_ = block;

}

