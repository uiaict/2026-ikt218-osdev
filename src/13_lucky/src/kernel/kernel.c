#include "stdio.h"
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "arch/i386/isr.h"
#include "kernel/memory.h"

// This symbol comes from linker.ld and it marks where the kernel ends in memory
extern uint32_t end;

void kernel_main(unsigned long magic, unsigned long multiboot_info) {
    init_gdt();
    init_idt();
    init_isr();
    enable_interrupts();

    // Initialize the kernel memory
    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();

    // printf test
    printf("Hello World\n");

    // malloc test
    void *a = malloc(12345);
    void *b = malloc(54321);
    void *c = malloc(13331);
    printf("malloc test: 0x%x 0x%x 0x%x\n", a, b, c);

    // Interrupt test
    __asm__ volatile ("int $0x0");
    __asm__ volatile ("int $0x1");
    __asm__ volatile ("int $0x2");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
