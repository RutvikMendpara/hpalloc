# hpalloc (High Performance allocator)

This allocator is designed for research and experimentation in **high-performance domains** where:

* Allocation latency is critical
* Memory fragmentation is a secondary concern
* Workloads involve fixed-size, page-aligned allocations
* Predictable, sequential allocation behavior is preferred over general-purpose flexibility

It’s **not** intended for production use or general-purpose memory management.

## Arena API

```cpp
class Arena {
public:
    explicit Arena(size_t size_in_bytes); // Create arena, size must be page-aligned <= 256MB
    ~Arena();                            // Releases memory

    void* alloc_page();                  // Returns next 4KB-aligned page or nullptr if exhausted
    size_t pages_allocated() const;     // Number of pages allocated so far
    size_t total_pages() const;         // Total pages in the arena
};
````

* Allocations are page-sized (4KB) and sequential.
* No deallocation support yet.
* Throws on invalid constructor arguments or mmap failure.




