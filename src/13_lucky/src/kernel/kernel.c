#include "stdio.h"
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "arch/i386/isr.h"

void kernel_main(unsigned long magic, unsigned long multiboot_info) {
    init_gdt();
    init_idt();
    init_isr();
    enable_interrupts();

    printf("Hello World\n");

    __asm__ volatile ("int $0x0");
    __asm__ volatile ("int $0x1");
    __asm__ volatile ("int $0x2");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
