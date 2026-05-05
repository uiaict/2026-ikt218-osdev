#pragma once

#include <isr.h>

typedef void (*irq_handler_t)(registers_t *regs);

void     irq_init(uint16_t cs);
void     irq_install_handler(int irq, irq_handler_t handler);
void     irq_uninstall_handler(int irq);
uint32_t timer_get_ticks(void);

extern void irq0(void);  extern void irq1(void);  extern void irq2(void);
extern void irq3(void);  extern void irq4(void);  extern void irq5(void);
extern void irq6(void);  extern void irq7(void);  extern void irq8(void);
extern void irq9(void);  extern void irq10(void); extern void irq11(void);
extern void irq12(void); extern void irq13(void); extern void irq14(void);
extern void irq15(void);
