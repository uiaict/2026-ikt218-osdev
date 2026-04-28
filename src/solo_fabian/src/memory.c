#include <memory.h>
#include <terminal.h>
#include <libc/stddef.h>

#define HEAP_SIZE 1024 * 1024

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES 1024
#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE 0x2

/* Align the heap start so allocations land on clean addresses */
#define ALIGN4(value) (((value) + 3) & ~3)

static uint32_t heap_start;
static uint32_t heap_current;
static uint32_t heap_end;

static uint32_t page_directory[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
static uint32_t first_page_table[PAGE_TABLE_ENTRIES] __attribute__ ((aligned(PAGE_SIZE)));

struct memory_block {
    size_t size;
    struct memory_block* next;
};

static struct memory_block* free_list;

void init_kernel_memory(uint32_t* kernel_end) 
{
    heap_start = ALIGN4((uint32_t)kernel_end);
    heap_current = heap_start;
    heap_end = heap_start + HEAP_SIZE;

    free_list = NULL;
}

/* tiny helper to print hex numbers. */
static void print_hex(uint32_t value)
{
    const char* hex = "0123456789ABCDEF";

    terminal_write("0x");
    
    for (int i = 28; i >= 0; i -= 4) {
        terminal_putchar(hex[(value >> i) & 0xF]);
    }
}

void print_pointer(const char* label, void* pointer)
{
    terminal_write(label);
    print_hex((uint32_t)pointer);
    terminal_putchar('\n');
}

void print_memory_layout(void)
{
    terminal_write("Memory layout:\n");

    terminal_write("kernel end: ");
    print_hex(heap_start);
    terminal_putchar('\n');

    terminal_write("heap start: ");
    print_hex(heap_start);
    terminal_putchar('\n');

    terminal_write("heap current: ");
    print_hex(heap_current);
    terminal_putchar('\n');

    terminal_write("heap end: ");
    print_hex(heap_end);
    terminal_putchar('\n');
}


void* malloc(size_t size)
{

    
    if (size == 0) {
        return NULL;
    }

    size = ALIGN4(size);

    struct memory_block* previous = NULL;
    struct memory_block* current = free_list;

    while (current != NULL) {
        if (current->size >= size) {
            if (previous == NULL) {
                free_list = current->next;
            } else {
                previous->next = current->next;
            }
            
            return (void*)((uint8_t*)current + sizeof(struct memory_block));
        }

        previous = current;
        current = current->next;
    }

    uint32_t total_size = sizeof(struct memory_block) + size;

    if (heap_current + total_size > heap_end) {
        return NULL;
    }

    struct memory_block* block = (struct memory_block*)heap_current;
    block->size = size;
    block->next = NULL;

    heap_current += total_size;

    return (void*)((uint8_t*)block + sizeof(struct memory_block));
}

void free(void* pointer) 
{    
    if (pointer == NULL) {
        return;
    }

    struct memory_block* block = 
        (struct memory_block*)((uint8_t*)pointer - sizeof(struct memory_block));

    block->next = free_list;
    free_list = block;

}

void init_paging(void)
{
    for (size_t i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    for (size_t i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        page_directory[i] = 0;
    }

    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITABLE;

    __asm__ volatile ("mov %0, %%cr3" : : "r"(page_directory));

    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0));

    terminal_write("Paging enabled\n");
}

