#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "keyboard.h"
#include "screen.h"
#include "libc/stdio.h"

void kernel_main(void) {
    gdt_install();
    screen_clear();
screen_write("Hello World\n");

screen_write("1\n");
idt_install();

screen_write("2\n");
isr_install();

screen_write("3\n");
irq_install();

screen_write("4\n");
keyboard_install();

screen_write("5\n");
    idt_install();
    isr_install();
    irq_install();
    keyboard_install();

    screen_write("IDT/ISR/IRQ initialized\n");
    screen_write("Testing interrupts...\n");

    __asm__ volatile ("int $0");
    __asm__ volatile ("int $1");
    __asm__ volatile ("int $2");

    screen_write("Keyboard ready: ");

    __asm__ volatile ("sti");

    while (1) {
        __asm__ volatile ("hlt");
    }
}