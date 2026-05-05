#include "memory.h"

void terminal_write(const char* str);

static uint32_t heap_begin = 0;
static uint32_t heap_end = 0x01000000; // 16 MB
static uint32_t last_alloc = 0;
static uint32_t memory_used = 0;

static void* memset_local(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void init_kernel_memory(uint32_t* kernel_end) {
    heap_begin = ((uint32_t)kernel_end + 0xFFF) & ~0xFFF;
    last_alloc = heap_begin;

    memset_local((void*)heap_begin, 0, heap_end - heap_begin);

    //terminal_write("Kernel memory initialized\n");
}

void* malloc(size_t size) {
    if (size == 0) {
        return 0;
    }

    // Align to 4 bytes
    if (size % 4 != 0) {
        size += 4 - (size % 4);
    }

    unsigned char* mem = (unsigned char*)heap_begin;

    while ((uint32_t)mem < last_alloc) {
        alloc_t* block = (alloc_t*)mem;

        if (block->size == 0) {
            break;
        }

        if (block->status == 0 && block->size >= size) {
            block->status = 1;
            memory_used += block->size + sizeof(alloc_t);

            void* usable_memory = mem + sizeof(alloc_t);
            memset_local(usable_memory, 0, block->size);
            return usable_memory;
        }

        mem += sizeof(alloc_t) + block->size;
    }

    if (last_alloc + sizeof(alloc_t) + size >= heap_end) {
        terminal_write("malloc failed: out of memory\n");
        return 0;
    }

    alloc_t* block = (alloc_t*)last_alloc;
    block->status = 1;
    block->size = size;

    void* usable_memory = (unsigned char*)block + sizeof(alloc_t);

    last_alloc += sizeof(alloc_t) + size;
    memory_used += sizeof(alloc_t) + size;

    memset_local(usable_memory, 0, size);

    return usable_memory;
}

void free(void* ptr) {
    if (!ptr) {
        return;
    }

    alloc_t* block = (alloc_t*)((unsigned char*)ptr - sizeof(alloc_t));

    if (block->status == 1) {
        block->status = 0;
        memory_used -= block->size + sizeof(alloc_t);
    }
}

void print_memory_layout(void) {
   // terminal_write("Heap starts after kernel end\n");
}