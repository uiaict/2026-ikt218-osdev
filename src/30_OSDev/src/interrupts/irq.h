#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

// Function to remap the PIC (Programmable Interrupt Controller)
void pic_remap();

// General IRQ handler called from ISR
void irq_handler(int irq_num);

#endif