#include "arch/i386/isr.h"
#include "arch/i386/idt.h"
#include "libc/stdio.h"

#include "terminal.h"

static int isr_line = 0;

// Each number can have one function assigned to it.
static isr_t interrupt_handlers[256];

// Write one byte to I/O port
static inline void outb(uint16 port, uint8 value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// register a handler function with a given number
void register_interrupt_handler(uint8 n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

// handles printing for testing isr 
void isr_handler(registers_t* regs) {
    terminal_write("Interrupt triggered: ", 0x0F, 0, isr_line);
    terminal_write("   ", 0x0F, 21, isr_line);

    print_uint(regs->int_no, 0x0F, 21, isr_line);

    isr_line++;

    if (isr_line >= VGA_HEIGHT) {
        isr_line = 0;
    }
}


void irq_handler(registers_t* regs) {
    
    //Call registered IRQ handler if one exists.
    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }

    
    //Send EOI to PICs.
    //IRQ8-IRQ15 come from the slave PIC, so they need EOI to both.
    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);
}
