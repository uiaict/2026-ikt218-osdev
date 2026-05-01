#include "irq.h"
#include "include/print.h"
#include "include/io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

// A table of function pointers, one slot per IRQ (0-15)
// NULL means no handler registered for that IRQ
static void (*irq_handlers[16])() = {0};

// Register a custom handler for a given IRQ number.
// The PIT will use slot 0, keyboard uses slot 1, etc.
void irq_install_handler(uint32_t irq_no, void (*handler)()) {
    if (irq_no < 16) {
        irq_handlers[irq_no] = handler;
    }
}

void irq_init() {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

void irq_handler(uint32_t irq_no) {
    // If a handler is registered for this IRQ, call it.
    // This replaces the hardcoded if-checks with a flexible dispatch table.
    if (irq_no < 16 && irq_handlers[irq_no] != 0) {
        irq_handlers[irq_no]();
    }

    // For IRQs 8-15 (slave PIC), we must also notify the slave PIC
    if (irq_no >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    // Always notify the master PIC we are done
    outb(PIC1_COMMAND, PIC_EOI);
}