#ifndef IDT_H
#define IDT_H

#include <libc/stdint.h>

struct idt_entry {
    uint16_t base_low;      // lower 16 bits of handler address
    uint16_t selector;      // kernel code segment selector
    uint8_t  zero;          // always 0
    uint8_t  flags;         // type and attributes
    uint16_t base_high;     // upper 16 bits of handler address
} __attribute__((packed));

struct idt_pointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));


void loadIDT(struct idt_pointer* idt_ptr);
void idt_init();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

#endif