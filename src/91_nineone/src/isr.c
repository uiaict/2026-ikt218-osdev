#include "isr.h"
#include "idt.h"

#include "terminal.h"

static int isr_line = 0;

static isr_t interrupt_handlers[256];

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void print_uint(uint32_t n, uint8_t color, int x, int y) {
    char buffer[11];
    int i = 0;

    if (n == 0) {
        terminal_write("0", color, x, y);
        return;
    }

    while (n > 0) {
        buffer[i++] = '0' + (n % 10);
        n /= 10;
    }

    char out[12];
    int j = 0;

    while (i > 0) {
        out[j++] = buffer[--i];
    }

    out[j] = '\0';

    terminal_write(out, color, x, y);
}


void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

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
    terminal_write("IRQ triggered: ", 0x0F, 0, isr_line);
    terminal_write("   ", 0x0F, 15, isr_line);

    print_uint(regs->int_no, 0x0F, 15, isr_line);

    isr_line++;

    if (isr_line >= VGA_HEIGHT) {
        isr_line = 0;
    }

    /*
     * Optional registered handler.
     * Useful later for keyboard IRQ1.
     */
    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }

    /*
     * Send EOI to PICs.
     * IRQ8-IRQ15 come from the slave PIC, so they need EOI to both.
     */
    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);
}