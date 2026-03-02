#include "gdt.h"
#include "terminal.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"


void main(void)
{
    gdt_init();                // Set up Global Descriptor Table
    idt_init();                // Set up Interrupt Descriptor Table
    pic_remap();              // Remap PIC before setting IRQs
    terminal_write("Hello World, IRQ setup complete!\n"); // Write to the screen

    asm volatile("int $0x0");  // Trigger interrupt 0
    asm volatile("int $0x1");  // Trigger interrupt 1
    asm volatile("int $0x2");  // Trigger interrupt 2 
    
    asm volatile("int $0x20"); // IRQ0
    asm volatile("int $0x21"); // IRQ1
    asm volatile("int $0x22"); // IRQ2
        
    while(1) { }                     // Halt CPU
}