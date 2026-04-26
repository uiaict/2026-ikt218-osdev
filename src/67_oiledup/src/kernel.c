#include "gdt/gdt.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/string.h"
#include "libc/stdio.h"
#include <multiboot2.h>
#include "idt/idt.h"
#include "kernel/memory.h"
#include "kernel/pit.h"

struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

extern uint32_t end;

int kernel_main();

int main(uint32_t magic, struct multiboot_info* mb_info_addr) {
    (void)magic;
    (void)mb_info_addr;

    printf("Starting up...\n");
    
    gdt_install();
    printf("[kernel] GDT installed\n");

    idt_init();
    printf("IDT Starting");

    asm volatile ("sti");

    init_kernel_memory(&end);

    init_paging();

    print_memory_layout();

    init_pit();
    
    char str[] = "Hello, world!";

    print(str, strlen(str));
    putchar('\n');
    
    printf("[kernel] Video+serial output written, calling kernel_main()\n");

    // Allocation test
    void* some_memory = malloc(1024);
    printf("Allocated at: 0x%x\n", some_memory);

    int counter = 0;

    // Test interval timer
    while(true){
        printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%d]: Slept using busy-waiting.\n", counter++);

        printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%d]: Slept using interrupts.\n", counter++);
};

    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}