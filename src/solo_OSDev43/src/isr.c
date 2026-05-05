#include "isr.h"

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);

void terminal_write(const char* str);
void idt_set_gate(unsigned char num, unsigned int base, unsigned short selector, unsigned char flags);

void isr_handler(unsigned int interrupt_number) {
    terminal_write("ISR triggered: ");

    if (interrupt_number == 0) terminal_write("0\n");
    else if (interrupt_number == 1) terminal_write("1\n");
    else if (interrupt_number == 2) terminal_write("2\n");
    else terminal_write("unknown\n");
}

void isr_install(void) {
    idt_set_gate(0, (unsigned int)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned int)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned int)isr2, 0x08, 0x8E);
}