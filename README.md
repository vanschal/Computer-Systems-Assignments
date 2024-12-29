# Computer-Systems-Assignments

# Directory 1: Parallel Matrix Multiplication

This project demonstrates the performance benefits of parallel computing through matrix multiplication. It compares the execution times of three different algorithms:
1. **Serial Matrix Multiplication**: A single-threaded implementation.
2. **Parallel Matrix Multiplication using Processes**: Leverages separate processes for parallelization.
3. **Parallel Matrix Multiplication using Threads**: Uses threads within a shared process for parallelization.

## Features
- Efficient handling of large matrices (e.g., 1024x1024).
- Implementation of matrix multiplication algorithms with flat arrays stored on the heap.
- Performance benchmarking and verification of results for correctness.
- Memory optimization using shared memory (for processes) and POSIX threads (for threading).

## File Structure
### 1. **matrix_mult.c**
   Contains the implementation of:
   - Serial matrix multiplication.
   - Process-based parallel matrix multiplication.
   - Thread-based parallel matrix multiplication.
   - Utility functions for initialization, timing, and verification.

### 2. **matrix_mult.h**
   - Symbolic constants and type definitions (e.g., `DIM`, `NUM_WORKERS`, `SUCCESS`, `FAILURE`).
   - Function prototypes and type aliases for modularity.

### 3. **main.c**
   - Entry point of the program.
   - Allocates and initializes matrices.
   - Calls matrix multiplication functions and measures execution time.
   - Verifies the correctness of results.

### 4. **Makefile**
   - Builds the project.
   - Includes options for compiling with `-std=gnu99` and linking with `-lpthread`.

## Algorithms
### Serial Matrix Multiplication
- Performs straightforward matrix multiplication without any parallelization.
- Used as a baseline for benchmarking and verifying parallel implementations.

### Parallel Matrix Multiplication (Processes)
- Divides the work among multiple processes using `fork`.
- Employs shared memory for combining results from child processes.
- Parent process waits for child processes to complete before verifying results.

### Parallel Matrix Multiplication (Threads)
- Divides the work among threads using POSIX threads (`pthreads`).
- Shares memory among threads for efficient data communication.
- Implements robust handling of edge cases, such as matrix dimensions not dividing evenly.


# Directory 2: Dynamic Memory Allocator

This project implements a custom dynamic memory allocator in C as a simplified alternative to the standard `malloc` package. It uses a doubly-linked list to manage free and allocated memory blocks, with additional features for coalescing adjacent free blocks and managing memory pages efficiently.

## Features
- **Custom Memory Management**: Allocates and frees memory using a doubly-linked list of memory blocks.
- **First-Fit Allocation Policy**: Quickly finds the first suitable block for memory requests.
- **Memory Coalescing**: Combines adjacent free blocks for efficient memory reuse.
- **Page Management**: Uses `mmap` to request memory pages and `munmap` to release unused pages.
- **Debugging Functions**: Includes utilities for printing memory states and headers for debugging purposes.

## File Structure
### 1. **mem_alloc.c**
   - Core implementation of the memory allocator.
   - Functions for memory initialization, allocation, and freeing.
   - Debugging utilities for testing and troubleshooting.

### 2. **mem_alloc.h**
   - Contains symbolic constants, type definitions, and function declarations for the public API.

### 3. **makefile**
   - Build automation script for compiling and testing the memory allocator.

### 4. **main.c**
   - Provided test driver for verifying the functionality of the allocator.
   - Demonstrates allocation, freeing, and coalescing operations.

## Key Functions
1. **`void *mem_alloc(size_t requested_size)`**
   - Allocates memory of at least `requested_size` bytes.
   - Returns a pointer to the allocated memory or `NULL` if allocation fails.

2. **`void mem_free(void *ptr)`**
   - Frees the memory block at the given pointer.
   - Coalesces adjacent free blocks when possible.

### Internal Functions
- **`int mem_init()`**: Initializes the first memory page and sets up the free list.
- **`int mem_extend(Header *last)`**: Extends the free list by adding a new memory page.
- **Helper Functions**:
  - `is_allocated`, `is_free`, `set_allocated`, `set_free`
  - `get_header`, `same_page`

### Debugging Utilities
- **`void print_list()`**: Prints the addresses of all headers in the free list.
- **`void print_header(Header *header)`**: Prints the details of a single memory block header.
- **`size_t get_size(Header *header)`**: Retrieves the size of a block without the allocation bit.
