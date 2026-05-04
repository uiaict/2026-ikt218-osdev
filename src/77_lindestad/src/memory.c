#include <memory.h>

#include <libc/stdio.h>

#define MAX_PAGE_ALIGNED_ALLOCS 32
#define PAGE_SIZE 4096
#define HEAP_LIMIT 0x3C0000
#define PHEAP_BEGIN 0x3C0000
#define PHEAP_END 0x400000
#define PAGE_DIRECTORY_ADDRESS 0x400000
#define PAGE_TABLE_START 0x404000

static uint32_t last_alloc;
static uint32_t heap_begin;
static uint32_t heap_end;
static uint32_t pheap_begin;
static uint32_t pheap_end;
static uint8_t* pheap_desc;
static uint32_t memory_used;

static uint32_t* page_directory;
static uint32_t* last_page_table;

static uint32_t align_up(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

void* memcpy(void* dest, const void* src, size_t count)
{
    uint8_t* dst = (uint8_t*)dest;
    const uint8_t* source = (const uint8_t*)src;

    for (size_t i = 0; i < count; i++) {
        dst[i] = source[i];
    }

    return dest;
}

void* memset(void* ptr, int value, size_t count)
{
    uint8_t* bytes = (uint8_t*)ptr;

    for (size_t i = 0; i < count; i++) {
        bytes[i] = (uint8_t)value;
    }

    return ptr;
}

void* memset16(void* ptr, uint16_t value, size_t count)
{
    uint16_t* words = (uint16_t*)ptr;

    for (size_t i = 0; i < count; i++) {
        words[i] = value;
    }

    return ptr;
}

void init_kernel_memory(uint32_t* kernel_end)
{
    heap_begin = align_up((uint32_t)kernel_end, PAGE_SIZE);
    last_alloc = heap_begin;
    heap_end = HEAP_LIMIT;
    pheap_begin = PHEAP_BEGIN;
    pheap_end = PHEAP_END;
    memory_used = 0;

    memset((void*)heap_begin, 0, heap_end - heap_begin);
    pheap_desc = (uint8_t*)malloc(MAX_PAGE_ALIGNED_ALLOCS);
    memset(pheap_desc, 0, MAX_PAGE_ALIGNED_ALLOCS);
}

void print_memory_layout(void)
{
    printf("memory layout\n");
    printf("  heap start: 0x%x\n", heap_begin);
    printf("  heap end:   0x%x\n", heap_end);
    printf("  pheap:      0x%x - 0x%x\n", pheap_begin, pheap_end);
    printf("  used:       %u bytes\n", memory_used);
    printf("  free:       %u bytes\n", heap_end - heap_begin - memory_used);
}

void free(void* memory)
{
    if (memory == 0) {
        return;
    }

    alloc_t* alloc = (alloc_t*)((uint32_t)memory - sizeof(alloc_t));
    if (alloc->status == 0) {
        return;
    }

    alloc->status = 0;
    memory_used -= alloc->size + sizeof(alloc_t);
}

void pfree(void* memory)
{
    uint32_t address = (uint32_t)memory;

    if (address < pheap_begin || address >= pheap_end) {
        return;
    }

    uint32_t page = (address - pheap_begin) / PAGE_SIZE;
    if (page < MAX_PAGE_ALIGNED_ALLOCS) {
        pheap_desc[page] = 0;
    }
}

char* pmalloc(size_t size)
{
    (void)size;

    for (uint32_t i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++) {
        if (pheap_desc[i] != 0) {
            continue;
        }

        pheap_desc[i] = 1;
        return (char*)(pheap_begin + i * PAGE_SIZE);
    }

    return 0;
}

void* malloc(size_t size)
{
    if (size == 0) {
        return 0;
    }

    size = align_up((uint32_t)size, 4);

    uint8_t* memory = (uint8_t*)heap_begin;
    while ((uint32_t)memory < last_alloc) {
        alloc_t* alloc = (alloc_t*)memory;
        if (alloc->size == 0) {
            break;
        }

        if (alloc->status == 0 && alloc->size >= size) {
            alloc->status = 1;
            memory_used += alloc->size + sizeof(alloc_t);
            memset(memory + sizeof(alloc_t), 0, alloc->size);
            return memory + sizeof(alloc_t);
        }

        memory += sizeof(alloc_t) + alloc->size;
    }

    if (last_alloc + sizeof(alloc_t) + size >= heap_end) {
        printf("malloc: out of kernel heap\n");
        return 0;
    }

    alloc_t* alloc = (alloc_t*)last_alloc;
    alloc->status = 1;
    alloc->size = (uint32_t)size;

    last_alloc += sizeof(alloc_t) + (uint32_t)size;
    memory_used += sizeof(alloc_t) + (uint32_t)size;

    void* result = (void*)((uint32_t)alloc + sizeof(alloc_t));
    memset(result, 0, size);
    return result;
}

void paging_map_virtual_to_phys(uint32_t virtual_address, uint32_t physical_address)
{
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t* page_table = last_page_table;

    for (uint32_t i = 0; i < 1024; i++) {
        page_table[i] = physical_address | 0x3;
        physical_address += PAGE_SIZE;
    }

    page_directory[page_directory_index] = ((uint32_t)page_table) | 0x3;
    last_page_table = (uint32_t*)((uint32_t)last_page_table + PAGE_SIZE);
}

static void paging_enable(void)
{
    uint32_t page_directory_address = (uint32_t)page_directory;

    __asm__ volatile("mov %0, %%cr3" : : "r"(page_directory_address));

    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

void init_paging(void)
{
    page_directory = (uint32_t*)PAGE_DIRECTORY_ADDRESS;
    last_page_table = (uint32_t*)PAGE_TABLE_START;

    memset(page_directory, 0, PAGE_SIZE);
    memset(last_page_table, 0, PAGE_SIZE * 2);

    paging_map_virtual_to_phys(0x00000000, 0x00000000);
    paging_map_virtual_to_phys(0x00400000, 0x00400000);
    paging_enable();

    printf("paging enabled\n");
}
