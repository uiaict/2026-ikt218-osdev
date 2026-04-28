#include "kernel/idt.h"
#include "libc/system.h"
#include "kernel/keyboard.h"

struct idt_entry idt[256];
struct idtr idtr;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

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

    // Exceptions
    idt_set_entry(&idt[0], (uint32_t)isr0, 0x08, 0x8E);
    idt_set_entry(&idt[1], (uint32_t)isr1, 0x08, 0x8E);
    idt_set_entry(&idt[2], (uint32_t)isr2, 0x08, 0x8E);
    idt_set_entry(&idt[3], (uint32_t)isr3, 0x08, 0x8E);
    idt_set_entry(&idt[4], (uint32_t)isr4, 0x08, 0x8E);
    idt_set_entry(&idt[5], (uint32_t)isr5, 0x08, 0x8E);
    idt_set_entry(&idt[6], (uint32_t)isr6, 0x08, 0x8E);
    idt_set_entry(&idt[7], (uint32_t)isr7, 0x08, 0x8E);
    idt_set_entry(&idt[8], (uint32_t)isr8, 0x08, 0x8E);
    idt_set_entry(&idt[9], (uint32_t)isr9, 0x08, 0x8E);
    idt_set_entry(&idt[10], (uint32_t)isr10, 0x08, 0x8E);
    idt_set_entry(&idt[11], (uint32_t)isr11, 0x08, 0x8E);
    idt_set_entry(&idt[12], (uint32_t)isr12, 0x08, 0x8E);
    idt_set_entry(&idt[13], (uint32_t)isr13, 0x08, 0x8E);
    idt_set_entry(&idt[14], (uint32_t)isr14, 0x08, 0x8E);
    idt_set_entry(&idt[15], (uint32_t)isr15, 0x08, 0x8E);
    idt_set_entry(&idt[16], (uint32_t)isr16, 0x08, 0x8E);
    idt_set_entry(&idt[17], (uint32_t)isr17, 0x08, 0x8E);
    idt_set_entry(&idt[18], (uint32_t)isr18, 0x08, 0x8E);
    idt_set_entry(&idt[19], (uint32_t)isr19, 0x08, 0x8E);
    idt_set_entry(&idt[20], (uint32_t)isr20, 0x08, 0x8E);
    idt_set_entry(&idt[21], (uint32_t)isr21, 0x08, 0x8E);
    idt_set_entry(&idt[22], (uint32_t)isr22, 0x08, 0x8E);
    idt_set_entry(&idt[23], (uint32_t)isr23, 0x08, 0x8E);
    idt_set_entry(&idt[24], (uint32_t)isr24, 0x08, 0x8E);
    idt_set_entry(&idt[25], (uint32_t)isr25, 0x08, 0x8E);
    idt_set_entry(&idt[26], (uint32_t)isr26, 0x08, 0x8E);
    idt_set_entry(&idt[27], (uint32_t)isr27, 0x08, 0x8E);
    idt_set_entry(&idt[28], (uint32_t)isr28, 0x08, 0x8E);
    idt_set_entry(&idt[29], (uint32_t)isr29, 0x08, 0x8E);
    idt_set_entry(&idt[30], (uint32_t)isr30, 0x08, 0x8E);
    idt_set_entry(&idt[31], (uint32_t)isr31, 0x08, 0x8E);

    // IRQs
    idt_set_entry(&idt[32], (uint32_t)irq0, 0x08, 0x8E);
    idt_set_entry(&idt[33], (uint32_t)irq1, 0x08, 0x8E);
    idt_set_entry(&idt[34], (uint32_t)irq2, 0x08, 0x8E);
    idt_set_entry(&idt[35], (uint32_t)irq3, 0x08, 0x8E);
    idt_set_entry(&idt[36], (uint32_t)irq4, 0x08, 0x8E);
    idt_set_entry(&idt[37], (uint32_t)irq5, 0x08, 0x8E);
    idt_set_entry(&idt[38], (uint32_t)irq6, 0x08, 0x8E);
    idt_set_entry(&idt[39], (uint32_t)irq7, 0x08, 0x8E);
    idt_set_entry(&idt[40], (uint32_t)irq8, 0x08, 0x8E);
    idt_set_entry(&idt[41], (uint32_t)irq9, 0x08, 0x8E);
    idt_set_entry(&idt[42], (uint32_t)irq10, 0x08, 0x8E);
    idt_set_entry(&idt[43], (uint32_t)irq11, 0x08, 0x8E);
    idt_set_entry(&idt[44], (uint32_t)irq12, 0x08, 0x8E);
    idt_set_entry(&idt[45], (uint32_t)irq13, 0x08, 0x8E);
    idt_set_entry(&idt[46], (uint32_t)irq14, 0x08, 0x8E);
    idt_set_entry(&idt[47], (uint32_t)irq15, 0x08, 0x8E);

    idtr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtr.base  = (uint32_t)&idt;

    idt_load(&idtr);

    // Register keyboard handler
    register_interrupt_handler(33, keyboard_handler);
}
