#ifndef IRQ_H
#define IRQ_H

#include <libc/stdint.h>

void irq_init();
void irq_handler(uint32_t irq_no);

#endif