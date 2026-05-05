#include "kernel_memory.h"
#include <stdint.h>
#include <stddef.h>
#include "../terminal.h"

extern uint32_t end; 
static uint32_t* heap_start = 0;
static uint32_t* heap_end = 0;

void init_kernel_memory(uint32_t* kernel_end) {
    heap_start = kernel_end;  // Start på ledig minne etter kjernen
    heap_end = heap_start;
}

void* malloc(uint32_t size) {
    void* ptr = (void*)heap_start;
    heap_start += (size + 3) / 4; // Align to 4-byte boundaries
     if (heap_start > heap_end) heap_end = heap_start;
    return ptr;
}

void free(void* ptr) {
 
}

void print_memory_layout(void) {
    terminal_write("Memory Layout:\n");
    terminal_write("Kernel ends at: 0x");
    terminal_write_hex((uint32_t)&end);
    terminal_write("\n");
    terminal_write("Heap starts at: 0x");
    terminal_write_hex((uint32_t)heap_start);
    terminal_write("\n");
    terminal_write("Heap ends at: 0x");
    terminal_write_hex((uint32_t)heap_end);
    terminal_write("\n");
}
