#include <memory.h>
#include <terminal.h>
#include <libc/stddef.h>

#define HEAP_SIZE 1024 * 1024

/* Align the heap start so allocations land on clean addresses */
#define ALIGN4(value) (((value) + 3) & ~3)

static uint32_t heap_start;
static uint32_t heap_current;
static uint32_t heap_end;

void init_kernel_memory(uint32_t* kernel_end) 
{
    heap_start = ALIGN4((uint32_t)kernel_end);
    heap_current = heap_start;
    heap_end = heap_start + HEAP_SIZE;
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

    if (heap_current + size > heap_end) {
        return NULL;
    }

    void* allocated_memory = (void*)heap_current;
    heap_current += size;

    return allocated_memory;
}

void free(void* pointer) 
{
    (void*)pointer;
}
