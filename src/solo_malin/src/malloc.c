#include "memory.h"
#include <libc/stdio.h>
#include "screen.h"
#include "util.h"

// ==============================
// Kernel heap allocator
//
// Provides simple malloc/free for
// the kernel and page-aligned
// allocations via pmalloc/pfree.
// ==============================

#define MAX_PAGE_ALIGNED_ALLOCS 32

extern void panic(const char* message);

// Heap state
uint32_t last_alloc   = 0;   // End of last allocation (current heap top)
uint32_t heap_end     = 0;   // End of heap region
uint32_t heap_begin   = 0;   // Start of heap region

// Page-aligned heap (pheap) state
uint32_t pheap_begin  = 0;   // Start of page-aligned area
uint32_t pheap_end    = 0;   // End of page-aligned area
uint8_t* pheap_desc   = 0;   // Descriptor array (one byte per page slot)

uint32_t memory_used  = 0;   // Total bytes currently allocated

// Initialize kernel heap and page-aligned heap
void init_kernel_memory(uint32_t* kernel_end){

    // Start heap after kernel image (+ 4 KB padding)
    last_alloc = (uint32_t)kernel_end + 0x1000;
    heap_begin = last_alloc;

    // Reserve top of region for page-aligned allocations
    pheap_end = 0x400000;
    pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
    heap_end = pheap_begin;

    // Clear heap region
    memset((char*)heap_begin, 0, heap_end - heap_begin);

    // Allocate descriptor array for page-aligned blocks
    pheap_desc = (uint8_t*)malloc(MAX_PAGE_ALIGNED_ALLOCS);
    write_string("Kernel heap initialized\n");
    //write_string("Kernel heap starts at 0x%x\n", last_alloc);
}

// Print basic heap layout and usage
void print_memory_layout(void){

    kprintf("Memory used: %d bytes\n", memory_used);
    kprintf("Memory free %d bytes\n", heap_end - heap_begin - memory_used);
    kprintf("Heap size: %d bytes\n", heap_end - heap_begin);
    kprintf("Heap start: %x\n", heap_begin);
    kprintf("Heap end: %x\n", heap_end);
    kprintf("PHeap start: %x\n", pheap_begin);
    kprintf("PHeap end: %x\n", pheap_end);
}

// Free a normal heap allocation
void free(void* mem){

    if (!mem) return;

    // Move back to allocation header
    alloc_t* alloc = (alloc_t*)((uint8_t*)mem - sizeof(alloc_t));
    memory_used -= alloc->size +  sizeof(alloc_t);
    alloc->status = 0;      // Mark as free (block may be reused)
}

// Free a page-aligned allocation
void pfree(void* mem){

    // Check if pointer belongs to pheap
    if ((uint32_t)mem < pheap_begin || (uint32_t)mem >= pheap_end)
        return;
    
    uint32_t addr = (uint32_t)mem;
    addr -= pheap_begin;
    addr /= 4096;           // Convert address to page index

    pheap_desc[addr] = 0;   // Mark page slot as free
}

// Allocate a page-aligned 4 KB block
char* pmalloc(size_t size){

    (void) size;        // Size is ignored (always one page)

    for (int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++){
        
        if (pheap_desc[i]) continue;        // Skip used slots

        pheap_desc[i] = 1;                  // Mark slot as used
        kprintf("PALLocated from 0x%x to 0x%x\n",
                pheap_begin + i * 4096,
                pheap_begin + (i + 1) * 4096);
        
        return (char*)(pheap_begin + i * 4096);
    }

    write_string("pmalloc: FATAL: failure!\n");
    return 0;
}

// Simple first-fit heap allocator
void* malloc(size_t size){

    if (!size) return 0;
    
    uint8_t* mem = (uint8_t*)heap_begin;

    // Scan existing blocks to find a free/fit block
    while ((uint32_t)mem < last_alloc){
        alloc_t* a = (alloc_t*)mem;

        // Reached uninitialized space
        if (!a->size)
            goto nalloc;
        
        // Skip allocated block
        if (a->status){
            mem += a->size;
            mem += sizeof(alloc_t);
            mem += 4;       // Padding/alignment
            continue;
        }

        // Reuse free block if it is big enough
        if (a->size >= size){
            a->status = 1;
            kprintf("RE:Allocated %d bytes from 0x%x to 0x%x\n",
                    (int)size,
                    (uint32_t)(mem + sizeof(alloc_t)),
                    (uint32_t)(mem + sizeof(alloc_t) + size));

            memset(mem + sizeof(alloc_t), 0, size);
            memory_used += size + sizeof(alloc_t);
            return (void*)(mem + sizeof(alloc_t));
        }

        // Not big enough: skip to next block
        mem += a->size;
        mem += sizeof(alloc_t);
        mem += 4;
    }

nalloc:
    // No suitable free block: extend heap
    if (last_alloc  + size + sizeof(alloc_t) >= heap_end){
        panic("Cannot allocate bytes! Out of memory.\n");
        return 0;
    }

    // Set up new allocation header at last_alloc
    alloc_t* alloc = (alloc_t*)last_alloc;
    alloc->status = 1;
    alloc->size = size;

    // Move heap top past header + data + padding
    last_alloc += size;
    last_alloc += sizeof(alloc_t);
    last_alloc += 4;

    kprintf("Allocated %d bytes from %x to %x\n",
            (int)size,
            (uint32_t)alloc + sizeof(alloc_t),
            last_alloc);
    
    memory_used += size + 4 + sizeof(alloc_t);
    
    // Zero out allocated block
    memset((char*)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
    
    return (void*)((uint32_t)alloc + sizeof(alloc_t));

}