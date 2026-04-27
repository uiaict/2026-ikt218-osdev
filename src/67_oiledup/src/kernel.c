#include "gdt/gdt.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/string.h"
#include "libc/stdio.h"
#include <multiboot2.h>
#include "idt/idt.h"
#include "terminal.h"
#include "kernel/memory.h"
#include "kernel/pit.h"

struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

extern uint32_t end;

int kernel_main();
void terminal_initialize(void);

int main(uint32_t magic, struct multiboot_info* mb_info_addr) {
    (void)magic;
    (void)mb_info_addr;

    terminal_initialize();
    
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

    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}