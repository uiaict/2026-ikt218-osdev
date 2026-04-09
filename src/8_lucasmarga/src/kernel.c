#include "libc/stdint.h"
#include "terminal.h"
#include "gdt.h"

void kmain(uint32_t magic, void* mb_info_addr) {
    (void)magic;
    (void)mb_info_addr;

    gdt_setup();

    terminal_initialize();
    terminal_print_string("Hello World\n");

    while (1) {
        __asm__ __volatile__("hlt");
    }
}