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
    
    size_t page_size() const;
    void* alloc_page();                   // Returns next 4KB-aligned page or nullptr if exhausted
    size_t pages_allocated() const;
    size_t total_pages() const;
};
````

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
    void* allocate();
    void deallocate(void* ptr);

private:
    void refill(); // Gets a new page and splits into blocks
};
```

---

## Size-Class Allocator

A layer on top of `Fixed_Block`, supporting multiple block sizes (32, 64, 128, 256 bytes). Allocation size is rounded up to the nearest supported size class.

### Key properties:

* Maintains a separate `Fixed_Block` for each size class
* Internally manages a pool of arenas (1 per size class)
* Allocation size must be ≤ 256 bytes
* Fast lookup and reuse
* Deallocation requires caller to pass the correct size class used during allocation

### API

```cpp
class SizeClassAllocator {
public:
    explicit SizeClassAllocator(size_t arena_size_per_class = 1 << 20); // 1MB arena per class

    void* allocate(size_t size, size_t& actual_class_size); // Rounds up and returns block
    void deallocate(void* ptr, size_t actual_class_size);   // Caller must provide size used
};
```
---

### Benchmark Results (Block Size: 64, Count: 10,000,000)
> Benchmarked on: Fedora 42 · Kernel 6.14.8-300 · AMD Ryzen 5 3550H (4C/8T) · 16GB RAM

| Run | Allocator | Total Time (ms) | Avg Latency (ns) | P99 Latency (ns) | RSS (KB) |
| --- | --------- | --------------- | ---------------- | ---------------- | -------- |
| 1   | hpalloc   | 981.828         | 98.1828          | 51               | 160132   |
| 1   | malloc    | 2045.81         | 204.581          | 2986             | 941320   |
| 2   | hpalloc   | 979.79          | 97.979           | 51               | 160200   |
| 2   | malloc    | 1840.94         | 184.094          | 2966             | 941320   |
| 3   | hpalloc   | 980.181         | 98.0181          | 51               | 160164   |
| 3   | malloc    | 1828.4          | 182.84           | 2966             | 941256   |
| 4   | hpalloc   | 979.383         | 97.9383          | 51               | 160008   |
| 4   | malloc    | 1839.73         | 183.973          | 2966             | 941200   |
| 5   | hpalloc   | 980.125         | 98.0125          | 51               | 160196   |
| 5   | malloc    | 1856.51         | 185.651          | 2975             | 941200   |

---

## Notes

* These allocators are designed for performance and minimal overhead, not safety or general-purpose use.
* No bounds checking, memory ownership tracking, or protection against misuse is provided.
* Deallocating with an incorrect size or misusing an allocator will lead to undefined behavior.
* Thread safety is not guaranteed; locking must be handled externally if required.
* Intended for systems-level performance testing, allocator benchmarking, and research into allocation strategies, not production deployment.

---

## Build & Benchmarks

### Build

```bash
./build.sh
````

### Run Benchmarks

```bash
./benchmark.sh
```

### Requirements

* CMake ≥ 3.x
* g++ or clang++
* Linux (tested on Fedora)

