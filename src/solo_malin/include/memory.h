#ifndef MEMORY_H
#define MEMORY_H

#include <libc/stdint.h>
#include <libc/stddef.h>

// ==============================
// Memory management
//
// Provides functions for
// memory allocation, paging,
// and basic memory operations.
// ==============================

// Structure used for tracking memory allocations
typedef struct {
    uint8_t status;     // Allocation status (free/used)
    uint32_t size;      // Size of the memory block
} alloc_t;

// Initialize kernel memory (heap setup)
void init_kernel_memory(uint32_t* kernel_end);

// Initialize paging (virtual memory)
void init_paging(void);

// Map a virtual address to a physical address
void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys);

char* pmalloc(size_t size);     // Allocate physical memory
void* malloc(size_t size);      // Allocate memory (heap)
void free(void* mem);           // Free allocated memory

// Memory utility functions
void* memcpy(void* dest, const void* src, size_t num);      // Copy memory
void* memset(void* ptr, int value, size_t num);             // Set bytes
void* memset16(void* ptr, uint16_t value, size_t num);      // Set 16-bit values

// Debug: print memory layout
void print_memory_layout(void);

#endif
