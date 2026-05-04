#include <gdt.h>
#include <interrupts.h>
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
    interrupts_initialize();

    __asm__ volatile("int $0x0");
    __asm__ volatile("int $0x1");
    __asm__ volatile("int $0x2");

    printf("Keyboard input: ");
    __asm__ volatile("sti");

    for (;;) {
        __asm__ volatile("hlt");
    }
}
