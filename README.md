# hpalloc (High Performance Allocator)

This allocator is designed for **research and experimentation** in high-performance domains where:

* Allocation latency is critical
* Memory fragmentation is a secondary concern
* Workloads involve fixed-size, page-aligned allocations
* Predictable, sequential allocation behavior is preferred over general-purpose flexibility

**Not** intended for production use or general-purpose memory management.

---

## Arena Allocator

A low-level memory pool that provides 4KB page-aligned chunks.

### Key properties:

* Allocations are 4KB pages, handed out sequentially
* No deallocation support
* Throws on invalid size or `mmap` failure
* Maximum size: 256MB

### API

```cpp
class Arena {
public:
    explicit Arena(size_t size_in_bytes); // Create arena; size must be page-aligned and <= 256MB
    ~Arena();                             // Releases memory
    
    size_t page_size() const;             // Total size of page
    void* alloc_page();                   // Returns next 4KB-aligned page or nullptr if exhausted
    size_t pages_allocated() const;      // Number of pages allocated so far
    size_t total_pages() const;          // Total pages in the arena
};
```

---

## Fixed-Size Block Allocator

Built on top of `Arena`, this allocator chops pages into fixed-size blocks (e.g., 64 bytes) and manages them with a freelist.

### Key properties:

* Extremely fast allocation/deallocation of fixed-size blocks
* Uses a pointer-based freelist
* Constant-time deallocation
* No merging or compaction
* Block size must be aligned and ≥ sizeof(void\*)

### API

```cpp
class Fixed_Block {
public:
    Fixed_Block(Arena& arena, size_t block_size = 64); // Block size must be aligned
    void* allocate();                                  // Allocates a block
    void deallocate(void* ptr);                        // Returns block to freelist

private:
    void refill();                                     // Gets a new page and splits into blocks
};
```

