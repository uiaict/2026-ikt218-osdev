#include "kernel/memory.h"
#include "terminal.h"
#include "colors.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "kernel/system.h"

#define print terminal_write
#define MAX_PAGE_ALIGNED_ALLOCS 32

uint32 last_alloc = 0;
uint32 heap_end = 0;
uint32 heap_begin = 0;
uint32 pheap_begin = 0;
uint32 pheap_end = 0;
uint8* pheap_desc = 0;
uint32 memory_used = 0;

// Initialize the kernel memory manager
void init_kernel_memory(uint32* kernel_end)
{      
    // 0x1000 is just padding.
    last_alloc = (uint32)kernel_end + 0x1000;

    // Normal heap allocation is placed after kernel end (from line.Id)
    heap_begin = last_alloc;

    // 
    pheap_end = 0x400000;
    pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);

    // Page heap placed right after normal heap
    heap_end = pheap_begin;

    //clear heap
    memset((char *)heap_begin, 0, heap_end - heap_begin);

    // descriptor for tracking pages allocation
    pheap_desc = (uint8 *)malloc(MAX_PAGE_ALIGNED_ALLOCS);
}

// Print the current memory layout
void print_memory_layout()
{
    printf("Memory used: %u bytes", memory_used);
    printf("Memory free: %u bytes", heap_end - heap_begin - memory_used);
    printf("Heap size: %u bytes", heap_end - heap_begin);
    printf("Heap start: 0x%x", heap_begin); // change u with x (hexadecimal) later
    printf("Heap end: 0x%x", heap_end);
    printf("PHeap start: 0x%x, PHeap end: 0x%x", pheap_begin, pheap_end);   
}


// Free a block of memory
void free(void *mem)
{
    alloc_t *alloc =  (alloc_t *)((uint8 *)mem - sizeof(alloc_t));

    // Update use and mark the block free.
    memory_used -= alloc->size + sizeof(alloc_t);
    alloc->status = 0;
}

// Free a block of page-aligned memory
void pfree(void *mem)
{
    if(mem < pheap_begin || mem > pheap_end) return;

    // Determine the page ID
    uint32 ad = (uint32)mem;
    ad -= pheap_begin;
    ad /= 4096;

    // Set the page descriptor to free
    pheap_desc[ad] = 0;
}


// Allocate a block of page-aligned memory
char* pmalloc(size_t size)
{
    // Loop through the available list
    for(int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++)
    {
        if(pheap_desc[i]) continue;
        pheap_desc[i] = 1;
        return (char *)(pheap_begin + i*4096);
    }
    return 0;
}



// Allocate a block of memory
void* malloc(size_t size)
{
    if(!size) return 0;

    // Loop through blocks to find an available block with enough size
    uint8 *mem = (uint8 *)heap_begin;
    while((uint32)mem < last_alloc)
    {
        alloc_t *a = (alloc_t *)mem;

        if(!a->size)
            goto nalloc;
        if(a->status) {
            mem += a->size;
            mem += sizeof(alloc_t);
            mem += 4;
            continue;
        }
        // free block if it is large
        if(a->size >= size)
        {
            a->status = 1;
            printf("RE:Allocated %d bytes from 0x%x to 0x%x", size, mem + sizeof(alloc_t), mem + sizeof(alloc_t) + size);
            memset(mem + sizeof(alloc_t), 0, size);
            memory_used += size + sizeof(alloc_t);
            return (char *)(mem + sizeof(alloc_t));
        }
        // If the block is not allocated and its size is not big enough,
        // add its size and the sizeof(alloc_t) to the pointer and continue.
        mem += a->size;
        mem += sizeof(alloc_t);
        mem += 4;
    }

    nalloc:;
    // Allocate new block at the end of the heap.
    if(last_alloc + size + sizeof(alloc_t) >= heap_end)
    {
        panic("Cannot allocate bytes! Out of memory.\n");
    }
    alloc_t *alloc = (alloc_t *)last_alloc;
    alloc->status = 1;
    alloc->size = size;

    last_alloc += size;

    // Move heap pointer
    last_alloc += sizeof(alloc_t);
    last_alloc += 4;

    memory_used += size + 4 + sizeof(alloc_t);
    memset((char *)((uint32)alloc + sizeof(alloc_t)), 0, size); // Zero out allocated memory
    return (char *)((uint32)alloc + sizeof(alloc_t));
}

