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
    idt_init();
    init_kernel_memory(&end);
    init_paging();
    init_pit();

    asm volatile ("sti");
    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}