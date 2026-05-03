#include "memory.h"
#include <libc/stdio.h>
#include "screen.h"
#include "util.h"

#define MAX_PAGE_ALIGNED_ALLOCS 32

extern void panic(const char* message);

uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t* pheap_desc = 0;
uint32_t memory_used = 0;

void init_kernel_memory(uint32_t* kernel_end){

    last_alloc = (uint32_t)kernel_end + 0x1000;
    heap_begin = last_alloc;
    pheap_end = 0x400000;
    pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
    heap_end = pheap_begin;

    memset((char*)heap_begin, 0, heap_end - heap_begin);

    pheap_desc = (uint8_t*)malloc(MAX_PAGE_ALIGNED_ALLOCS);
    write_string("Kernel heap initialized\n");
    //write_string("Kernel heap starts at 0x%x\n", last_alloc);
}

void print_memory_layout(void){

    kprintf("Memory used: %d bytes\n", memory_used);
    kprintf("Memory free %d bytes\n", heap_end - heap_begin - memory_used);
    kprintf("Heap size: %d bytes\n", heap_end - heap_begin);
    kprintf("Heap start: %x\n", heap_begin);
    kprintf("Heap end: %x\n", heap_end);
    kprintf("PHeap start: %x\n", pheap_begin);
    kprintf("PHeap end: %x\n", pheap_end);
}

void free(void* mem){

    if (!mem) return;

    alloc_t* alloc = (alloc_t*)((uint8_t*)mem - sizeof(alloc_t));
    memory_used -= alloc->size +  sizeof(alloc_t);
    alloc->status = 0;
}

void pfree(void* mem){

    if ((uint32_t)mem < pheap_begin || (uint32_t)mem >= pheap_end)
        return;
    
    uint32_t addr = (uint32_t)mem;
    addr -= pheap_begin;
    addr /= 4096;

    pheap_desc[addr] = 0;
}

char* pmalloc(size_t size){

    (void) size;

    for (int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++){
        
        if (pheap_desc[i]) continue;

        pheap_desc[i] = 1;
        kprintf("PALLocated from 0x%x to 0x%x\n",
                pheap_begin + i * 4096,
                pheap_begin + (i + 1) * 4096);
        
        return (char*)(pheap_begin + i * 4096);
    }

    write_string("pmalloc: FATAL: failure!\n");
    return 0;
}


void* malloc(size_t size){

    if (!size) return 0;
    
    uint8_t* mem = (uint8_t*)heap_begin;

    while ((uint32_t)mem < last_alloc){
        alloc_t* a = (alloc_t*)mem;

        if (!a->size)
            goto nalloc;
        
        if (a->status){
            mem += a->size;
            mem += sizeof(alloc_t);
            mem += 4;
            continue;
        }

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

        mem += a->size;
        mem += sizeof(alloc_t);
        mem += 4;
    }

nalloc:
    if (last_alloc  + size + sizeof(alloc_t) >= heap_end){
        panic("Cannot allocate bytes! Out of memory.\n");
        return 0;
    }

    alloc_t* alloc = (alloc_t*)last_alloc;
    alloc->status = 1;
    alloc->size = size;

    last_alloc += size;
    last_alloc += sizeof(alloc_t);
    last_alloc += 4;

    kprintf("Allocated %d bytes from %x to %x\n",
            (int)size,
            (uint32_t)alloc + sizeof(alloc_t),
            last_alloc);
    
    memory_used += size + 4 + sizeof(alloc_t);
    memset((char*)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
    
    return (void*)((uint32_t)alloc + sizeof(alloc_t));

}