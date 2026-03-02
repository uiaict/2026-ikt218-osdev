#include "gdt.h"
#include "terminal.h"
#include "interrupts/idt.h"


void main(void)
{
    gdt_init();                // Set up GDT
    idt_init();                // Set up IDT

    terminal_write("Hello World\n"); // Write to the screen

    asm volatile("int $0x0");  // Trigger interrupt 0
    asm volatile("int $0x1");  // Trigger interrupt 1
    asm volatile("int $0x2");  // Trigger interrupt 2      
    while(1) { }                     // Halt CPU
}