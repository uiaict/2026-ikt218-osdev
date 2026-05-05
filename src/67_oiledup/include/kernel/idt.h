#ifndef IDT_H
#define IDT_H

#include "libc/stdint.h"

// A table for the different actions
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
} __attribute__((packed));

// IDTR Tells the cpu where to look
struct idtr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

typedef struct registers
{
   uint32_t ds;                                     // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pushad
   uint32_t int_no, err_code;                       // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss;           // Pushed by the processor automatically
} registers_t;

typedef void (*isr_t)(registers_t*);

extern struct idt_entry idt[256];
extern struct idtr idtr;

void idt_set_entry(struct idt_entry *entry, uint32_t handler, uint16_t selector, uint8_t type_attr);
void idt_init(void);
void register_interrupt_handler(uint8_t n, isr_t handler);

void isr0(void);
void isr8(void);
void isr14(void);
void isr32(void);
void isr33(void);

void idt_load(struct idtr *idtr);

#endif