#include <libc/stdint.h>
#include "gdt.h"
#include "terminal.h"
#include <descriptor_tables.h>
#include <interrupts.h>
#include <keyboard.h>

void main(uint32_t mb_magic, uint32_t mb_info_addr)
{
    (void)mb_magic;
    (void)mb_info_addr;

    gdt_init();

    terminal_initialize();
    terminal_write("Hello World\n");

    init_idt();
    init_irq();
    isr_register_default_prints();
    keyboard_init();

    __asm__ volatile ("int $0x0");
    __asm__ volatile ("int $0x3");
    __asm__ volatile ("int $0x4");

    __asm__ volatile ("sti");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
