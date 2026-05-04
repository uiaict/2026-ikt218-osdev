#ifndef IRQ_H
#define IRQ_H
#include <libc/stdint.h>

void irq_install();
void irq_handler_c(int irq);
#endif

