#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "kernel/memory.h"
#include "kernel/pit.h"
#include "kernel/terminal.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/string.h"
#include "libc/stdio.h"
#include "multiboot2.h"

struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

extern uint32_t end;

int kernel_main();
void terminal_initialize(void);

int main(uint32_t magic, struct multiboot_info* mb_info_addr) {
    // Initialize the terminal (screen output)
    terminal_initialize();
    // Initialize the Global Descriptor Table (GDT).
    init_gdt();
    // Initialize the Interrupt Descriptor Table (IDT).
    idt_init();
    // Now we can enable interupts again, since the IDT is set up.
    asm volatile ("sti");
    // Initialize the kernel's memory manager using the end address of the kernel.
    init_kernel_memory(&end);
    // Initialize paging for memory management.
    init_paging();
    // Initialize PIT
    init_pit();

    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}