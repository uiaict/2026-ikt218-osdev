#ifndef ISR_H
#define ISR_H

#include "libc/stdint.h"



#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct {
uint32 ds;

    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;

    uint32 int_no, err_code;

    uint32 eip, cs, eflags, useresp, ss;
} registers_t;



typedef void (*isr_t)(registers_t* regs);

void isr_handler(registers_t* regs);
void irq_handler(registers_t* regs);



void register_interrupt_handler(uint8 n, isr_t handler);


//Reserved ISR's for exceptions
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);

/* IRQs */
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);





void keyboard_callback(registers_t* regs);

#endif