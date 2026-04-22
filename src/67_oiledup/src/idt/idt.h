#ifndef IDT_H
#define IDT_H

#include <stdint.h>

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

extern struct idt_entry idt[256];
extern struct idtr idtr;

void idt_set_entry(struct idt_entry *entry, uint32_t handler, uint16_t selector, uint8_t type_attr);
void idt_init(void);

void isr0(void);
void isr8(void);
void isr14(void);

void idt_load(struct idtr *idtr);

#endif