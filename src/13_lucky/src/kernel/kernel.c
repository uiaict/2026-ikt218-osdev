#include "stdbool.h"
#include "stdio.h"
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "arch/i386/isr.h"
#include "kernel/memory.h"
#include "kernel/pit.h"

// This symbol comes from linker.ld and it marks where the kernel ends in memory
extern uint32_t end;

extern void test_new_operator(void);

void kernel_main(unsigned long magic, unsigned long multiboot_info) {
    init_gdt();
    init_idt();
    init_isr();
    init_pit();
    enable_interrupts();

    // Initialize the kernel memory and paging
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
    free(b);
    printf("free test: released 0x%x\n", b);

    test_new_operator();

    // Interrupt test
    __asm__ volatile ("int $0x0");
    __asm__ volatile ("int $0x1");
    __asm__ volatile ("int $0x2");

    uint32_t counter = 0;
    while (true) {
        // Assignment 4 PIT demo: compare active spinning with interrupt-driven sleeping.
        printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%d]: Slept using busy-waiting.\n", counter++);

        printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%d]: Slept using interrupts.\n", counter++);
    }
}
