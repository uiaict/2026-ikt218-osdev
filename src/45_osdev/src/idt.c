#include "../include/idt.h"
#include "../include/vga.h"   
#include <../include/libc/stdint.h>

extern void lidt(void*); // assembly stub that does lidt with pointer

// Total number of IDT entries - covers all CPU exceptions (0-31)
// and hardware IRQs remapped to 32-47, plus the rest unused
#define IDT_ENTRIES 256

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtp;

// Internal helper - fills a single IDT entry with the given handler, segment selector and flag
static void set_idt_entry(int n, uint32_t handler, uint16_t sel, uint8_t flags) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector   = sel;
    idt[n].zero       = 0;
    idt[n].type_attr  = flags;
    idt[n].offset_high= (handler >> 16) & 0xFFFF;
}

// Public version used by irq.c to register IRQ handlers (32-47) after remapping the PIC
// Uses kernel code selector 0x08 and interrupt gate flags 0x8E
void set_idt_entry_public(int n, uint32_t handler) {
    uint16_t code_sel = 0x08;
    uint8_t intr_flags = 0x8E;
    set_idt_entry(n, handler, code_sel, intr_flags);
}

// these are ISR stubs defined in isr.asm
extern void isr0();
extern void isr1();
extern void isr2();

void idt_install(void) {
    // Set up the IDT pointer struct - limit is size of table - 1
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base  = (uint32_t)&idt;

    // zero the table
    for (int i = 0; i < IDT_ENTRIES; ++i) {
        set_idt_entry(i, 0, 0, 0);
    }

    // This is the kernel segment selector
    uint16_t code_sel = 0x08;
    // 0x8E = present, ring 0, 32-bit interrupt gate.
    uint8_t intr_flags = 0x8E;

    set_idt_entry(0,  (uint32_t)isr0, code_sel, intr_flags);
    set_idt_entry(1,  (uint32_t)isr1, code_sel, intr_flags);
    set_idt_entry(2,  (uint32_t)isr2, code_sel, intr_flags);

    // load IDT
    lidt(&idtp);
}

