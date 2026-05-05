#ifndef IRQ_H
#define IRQ_H

#include "libc/stdint.h"

void irq_init(void);
void irq_register_handler(uint8_t irq, void (*handler)(void));

#endif