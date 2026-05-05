#include "irq.h"
#include "util.h"

// ==============================
// IRQ handling (C side)
//
// Manages custom handlers for
// hardware interrupts (IRQs).
// ==============================


// Table of custom IRQ handler functions (one per IRQ 0–15)
void *irq_routines[16] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

// Install a custom handler for a given IRQ line
void irq_install_handler (int irq, void (*handler)(struct  InterruptRegisters *r)){
    irq_routines[irq] = handler;
}

// Remove a custom handler for a given IRQ line
void irq_uninstall_handler(int irq){
    irq_routines[irq] = 0;
}

// Main IRQ handler called from assembly (irq_common_stub)
void irq_handler(struct InterruptRegisters* r){
    void (*handler)(struct InterruptRegisters *r);

    // Look up handler based on interrupt number (IRQ0 starts at vector 32)
    handler = irq_routines[r->int_no - 32];

    // Call custom handler if installed
    if (handler) {
        handler(r);
    }

     // If the interrupt came from the slave PIC, send EOI to slave first
    if (r->int_no >= 40){
            outPortB(0xA0, 0x20);
    }

    // Always send EOI to master PIC
    outPortB(0x20, 0x20);
}

