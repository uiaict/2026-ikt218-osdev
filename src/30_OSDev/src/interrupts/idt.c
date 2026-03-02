#include "idt.h"

// The IDT table
struct idt_entry idt[256];
struct idt_ptr idt_ptr;

extern void idt_flush(uint32_t);
extern void isr0();
extern void isr1();
extern void isr2();

// Define the function that fills in one row in the IDT.
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].selector = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_init()
{
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;  // Size of the IDT
    idt_ptr.base = (uint32_t)&idt;   // Address of the IDT in RAM

    // Set ISRs
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);

    idt_flush((uint32_t)&idt_ptr);  // Load the IDT table, calls assembly code
}