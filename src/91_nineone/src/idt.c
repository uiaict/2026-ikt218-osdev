#include "idt.h"
#include "isr.h"



__attribute__((aligned(0x10))) 
static idt_entry_t idt[IDT_MAX_DESCRIPTORS]; // Create an array of IDT entries; aligned for performance

static idtr_t idtr;

extern void* isr_stub_table[];


/* IRQ stubs from assembly */
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

static void idt_memset(void* dest, uint8_t value, uint32_t length) {
    uint8_t* ptr = (uint8_t*) dest;

    for (uint32_t i = 0; i < length; i++) {
        ptr[i] = value;
    }
}


static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void pic_remap(void) {
    /*
     * Remap PIC interrupts:
     * IRQ0-IRQ7  -> IDT 32-39
     * IRQ8-IRQ15 -> IDT 40-47
     */

    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20); // Master PIC offset = 32
    outb(0xA1, 0x28); // Slave PIC offset = 40

    outb(0x21, 0x04); // Tell master PIC there is a slave at IRQ2
    outb(0xA1, 0x02); // Tell slave PIC its cascade identity

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /*
     * Unmask all IRQs for now.
     * Later you may want to mask unused IRQs.
     */
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08;
    descriptor->attributes = flags;
    descriptor->isr_high = ((uint32_t)isr >> 16) & 0xFFFF;
    descriptor->reserved = 0;
}

void idt_init(void) {
    idtr.base = (uint32_t)&idt[0];
    idtr.limit = (uint16_t)(sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1);

    idt_memset(&idt, 0, sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS);

    /*
     * Your current CPU exception ISRs.
     */
    for (uint8_t vector = 0; vector < 4; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    }

    /*
     * Remap the PIC before installing IRQ gates.
     */
    pic_remap();

    /*
     * IRQ0-IRQ15 are mapped to IDT entries 32-47.
     */
    idt_set_descriptor(32, irq0, 0x8E);
    idt_set_descriptor(33, irq1, 0x8E);
    idt_set_descriptor(34, irq2, 0x8E);
    idt_set_descriptor(35, irq3, 0x8E);
    idt_set_descriptor(36, irq4, 0x8E);
    idt_set_descriptor(37, irq5, 0x8E);
    idt_set_descriptor(38, irq6, 0x8E);
    idt_set_descriptor(39, irq7, 0x8E);
    idt_set_descriptor(40, irq8, 0x8E);
    idt_set_descriptor(41, irq9, 0x8E);
    idt_set_descriptor(42, irq10, 0x8E);
    idt_set_descriptor(43, irq11, 0x8E);
    idt_set_descriptor(44, irq12, 0x8E);
    idt_set_descriptor(45, irq13, 0x8E);
    idt_set_descriptor(46, irq14, 0x8E);
    idt_set_descriptor(47, irq15, 0x8E);

    __asm__ volatile ("lidt %0" : : "m"(idtr));

    /*
     * Enable hardware interrupts.
     * Without this, IRQs will not fire.
     */
    __asm__ volatile ("sti");
}