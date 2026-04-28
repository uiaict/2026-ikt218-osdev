/* GDT setup must happen before the kernel relies on its own segment layout. */
#include <gdt.h>

/* IDT setup allows the kernel to receive CPU interrupts and hardware IRQs. */
#include <idt.h>

/* Terminal output is shared by the kernel and interrupt handlers. */
#include <terminal.h>

/* Kernel entry point called from multiboot2.asm. */
void main(void) {
    /* Install the kernel's GDT before doing regular kernel work. */
    gdt_init();

    /* Start from a blank VGA text screen. */
    terminal_clear();

    /* Required visible startup message. */
    terminal_write("Hello World\n");

    /* Install the IDT and configure IRQ0..IRQ15 through the PIC. */
    idt_init();

    /* Trigger three software interrupts to show that the ISR path works. */
    __asm__ volatile ("int $0x0");
    __asm__ volatile ("int $0x1");
    __asm__ volatile ("int $0x2");

    /* Enable maskable hardware interrupts, including timer and keyboard IRQs. */
    __asm__ volatile ("sti");

    /* Halt forever... Interrupts will wake the CPU when hardware events arrive. */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
