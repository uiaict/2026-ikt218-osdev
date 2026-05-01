#ifndef IRQ_H
#define IRQ_H

#include <libc/stdint.h>

void irq_init();
void irq_handler(uint32_t irq_no);

// Register a function to be called when a specific IRQ fires
void irq_install_handler(uint32_t irq_no, void (*handler)());

#endif

