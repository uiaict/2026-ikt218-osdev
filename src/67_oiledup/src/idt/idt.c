#include "idt.h"

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
    idt_set_entry(&idt[0],  (uint32_t)isr0,  0x08, 0x8E);
    idt_set_entry(&idt[8], (uint32_t)isr8, 0x08, 0x8E);
    idt_set_entry(&idt[14], (uint32_t)isr14, 0x08, 0x8E);

    idtr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtr.base  = (uint32_t)&idt;

    idt_load(&idtr);
}