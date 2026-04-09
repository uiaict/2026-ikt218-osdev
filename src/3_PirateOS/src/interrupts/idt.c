#include "interrupts/idt.h"

// Define the IDT with 256 entries.
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

// Declared in interrupts.asm
extern void idt_flush(uint32_t);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_low  = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_entries[num].flags   = flags /* | 0x60 */;
}

void init_idt()
{
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    // Clear the IDT memory to 0
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Load the IDT pointer
    idt_flush((uint32_t)&idt_ptr);
}
