#ifndef IRQ_H
#define IRQ_H

#include "isr.h"

// ==============================
// Interrupt Requests (IRQ)
//
// Defines functions and handlers
// for hardware interrupts.
//
// IRQs are signals from hardware
// devices (e.g., keyboard, timer)
// that require CPU attention.
// ==============================

// Main IRQ handler called from assembly
void irq_handler(struct InterruptRegisters *r);

// Install a custom handler for a specific IRQ
void irq_install_handler(int irq, void (*handler)(struct InterruptRegisters *r));

// Remove a handler for a specific IRQ
void irq_uninstall_handler(int irq);

// Assembly interrupt stubs for IRQs 0–15
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif