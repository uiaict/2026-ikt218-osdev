#include "memory.h"
#include "screen.h"

typedef struct block_header {
    size_t size;
    int free;
    struct block_header* next;
} block_header_t;

#define HEAP_SIZE_BYTES (1024 * 1024)
#define ALIGN4(x) (((x) + 3) & ~3)

static uint8_t* heap_start = 0;
static uint8_t* heap_end = 0;
static block_header_t* free_list = 0;

void init_kernel_memory(uint32_t* kernel_end) {
    uint32_t start = (uint32_t)kernel_end;
    start = (start + 0xFFF) & ~0xFFF;

    heap_start = (uint8_t*)start;
    heap_end = heap_start + HEAP_SIZE_BYTES;

    free_list = (block_header_t*)heap_start;
    free_list->size = HEAP_SIZE_BYTES - sizeof(block_header_t);
    free_list->free = 1;
    free_list->next = 0;
}

static void split_block(block_header_t* block, size_t size) {
    if (block->size <= size + sizeof(block_header_t)) {
        return;
    }

    block_header_t* new_block = (block_header_t*)((uint8_t*)(block + 1) + size);
    new_block->size = block->size - size - sizeof(block_header_t);
    new_block->free = 1;
    new_block->next = block->next;

    block->size = size;
    block->next = new_block;
}

void* malloc(size_t size) {
    if (size == 0 || free_list == 0) {
        return 0;
    }

    size = ALIGN4(size);

    block_header_t* current = free_list;
    while (current) {
        if (current->free && current->size >= size) {
            split_block(current, size);
            current->free = 0;
            return (void*)(current + 1);
        }
        current = current->next;
    }

    return 0;
}

static void coalesce_free_blocks(void) {
    block_header_t* current = free_list;
    while (current && current->next) {
        if (current->free && current->next->free) {
            current->size += sizeof(block_header_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void free(void* ptr) {
    if (!ptr) {
        return;
    }

    block_header_t* block = ((block_header_t*)ptr) - 1;
    block->free = 1;
    coalesce_free_blocks();
}

void print_memory_layout(void) {
    screen_write("Kernel heap start: ");
    screen_write_hex((uint32_t)heap_start);
    screen_write("\nKernel heap end:   ");
    screen_write_hex((uint32_t)heap_end);
    screen_write("\nKernel heap size:  ");
    screen_write_dec(HEAP_SIZE_BYTES);
    screen_write(" bytes\n");
}