#include <mem.h>
#include <libc/stdint.h>
#include <libc/stddef.h>
#include <libc/stdio.h>

#define HEAP_SIZE (1024 * 1024)  // 1 MB heap



typedef struct block_t {
    size_t         size;
    int            free;
    struct block_t *next;
} block_t;

static block_t  *heap        = NULL;
static uint32_t  heap_base   = 0;
static uint32_t  heap_limit  = 0;
static uint32_t  kernel_end_addr = 0;


#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITE   (1 << 1)
#define PAGE_PS      (1 << 7)   // 4 MB page (PSE)

static uint32_t page_directory[1024] __attribute__((aligned(4096)));



void init_kernel_memory(uint32_t *kernel_end) {
    kernel_end_addr = (uint32_t)kernel_end;

    // Align heap start to 4-byte boundary
    heap_base  = (kernel_end_addr + 3) & ~3u;
    heap_limit = heap_base + HEAP_SIZE;

    // Place one large free block covering the whole heap region
    heap        = (block_t *)heap_base;
    heap->size  = HEAP_SIZE - sizeof(block_t);
    heap->free  = 1;
    heap->next  = NULL;
}

void *malloc(size_t size) {
    if (!heap || size == 0) return NULL;

    // Round up to 4-byte alignment
    size = (size + 3) & ~3u;

    block_t *curr = heap;
    while (curr) {
        if (curr->free && curr->size >= size) {
            // Split only if the remainder is big enough to be useful
            if (curr->size >= size + sizeof(block_t) + 4) {
                block_t *split  = (block_t *)((uint8_t *)curr + sizeof(block_t) + size);
                split->size     = curr->size - size - sizeof(block_t);
                split->free     = 1;
                split->next     = curr->next;
                curr->next      = split;
                curr->size      = size;
            }
            curr->free = 0;
            return (void *)((uint8_t *)curr + sizeof(block_t));
        }
        curr = curr->next;
    }
    return NULL;  
}

void free(void *ptr) {
    if (!ptr) return;

    block_t *block = (block_t *)((uint8_t *)ptr - sizeof(block_t));
    block->free = 1;

    // Coalesce adjacent free blocks (forward pass)
    block_t *curr = heap;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(block_t) + curr->next->size;
            curr->next  = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void init_paging(void) {
    // Enable PSE (4 MB page support) in CR4
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 4);
    __asm__ volatile ("mov %0, %%cr4" : : "r"(cr4));

    // Clear page directory
    for (int i = 0; i < 1024; i++)
        page_directory[i] = 0;

    
    for (int i = 0; i < 4; i++)
        page_directory[i] = (uint32_t)(i * 0x400000) | PAGE_PRESENT | PAGE_WRITE | PAGE_PS;

    // Load page directory into CR3
    __asm__ volatile ("mov %0, %%cr3" : : "r"((uint32_t)page_directory));

    // Enable paging by setting pg bit in cr0
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1u << 31);
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));
}

void print_memory_layout(void) {
    printf("[MEM] Kernel start : 0x%x\n", 0x100000);
    printf("[MEM] Kernel end   : 0x%x\n", kernel_end_addr);
    printf("[MEM] Heap start   : 0x%x\n", heap_base);
    printf("[MEM] Heap end     : 0x%x\n", heap_limit);
    printf("[MEM] Heap size    : %d KB\n\n", (int)(HEAP_SIZE / 1024));
}
