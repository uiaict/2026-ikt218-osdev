#include "idt.h"

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

struct idt_entry idt[256];
struct idtr idtr;

void idt_set_entry(struct idt_entry *entry, uint32_t handler, uint16_t selector, uint8_t type_attr) {
    entry->offset_low  = (uint16_t)(handler & 0xFFFF);
    entry->offset_mid  = (uint16_t)((handler >> 16) & 0xFFFF);
    entry->selector    = selector;
    entry->ist         = 0;
    entry->type_attr   = type_attr;
}

void idt_init(void) {
    
    // Remap the PIC
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0x00); outb(0xA1, 0x00);

    idt_set_entry(&idt[0],  (uint32_t)isr0,  0x08, 0x8E);
    idt_set_entry(&idt[8], (uint32_t)isr8, 0x08, 0x8E);
    idt_set_entry(&idt[14], (uint32_t)isr14, 0x08, 0x8E);
    idt_set_entry(&idt[32], (uint32_t)isr32, 0x08, 0x8E);
    idt_set_entry(&idt[33], (uint32_t)isr33, 0x08, 0x8E);

    idtr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtr.base  = (uint32_t)&idt;

    idt_load(&idtr);
}