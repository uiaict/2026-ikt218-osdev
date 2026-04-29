#ifndef ISR_H
#define ISR_H

#include "types.h"

typedef struct {
uint32_t ds;

    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    uint32_t int_no, err_code;

    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;


/// FIIIIIIXXXXXXXXXX
void ist_init();

void isr_handler(registers_t regs);


//Reserved ISR's for exceptions
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);



#endif