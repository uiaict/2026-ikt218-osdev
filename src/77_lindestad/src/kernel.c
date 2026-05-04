#include <gdt.h>
#include <libc/stdint.h>
#include <libc/stdio.h>
#include <terminal.h>

int main(uint32_t multiboot_magic, uint32_t multiboot_info_addr)
{
    (void)multiboot_magic;
    (void)multiboot_info_addr;

    gdt_initialize();
    terminal_initialize();
    printf("Hello World\n");

    for (;;) {
        __asm__ volatile("hlt");
    }
}
