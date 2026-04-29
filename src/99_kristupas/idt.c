#include "idt.h"
#include "include/print.h"

// The table - 256 entries, one for each possible interrupt
struct idt_entry idt[256];

// The pointer we hand to the CPU so it can find the table
struct idt_pointer idt_ptr;

// ISR stubs defined in idt.asm - we need their addresses to register them in the IDT
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();

// IRQ stubs defined in irq.asm
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


// Fills in one row of the IDT table
// The handler address is split across base_low and base_high, just like
// base is split across three fields in the GDT
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);        // lower 16 bits of handler address
    idt[num].base_high = (base >> 16) & 0xFFFF;  // upper 16 bits of handler address
    idt[num].selector  = selector;               // which code segment to use (0x08 = kernel)
    idt[num].zero      = 0;                      // always 0, reserved by CPU
    idt[num].flags     = flags;                  // gate type + privilege level
}

void idt_init() {
    // Tell the pointer how big the table is and where it lives in memory
    idt_ptr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idt_ptr.base  = (uint32_t)&idt;

    // Zero out the entire table first - unset gates won't fire
    // (same idea as the null descriptor in GDT)
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Register each ISR stub into the IDT
    // 0x08 = kernel code segment selector, 0x8E = interrupt gate at ring 0 privilege
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);

    idt_set_gate(0x20, (uint32_t)irq0,  0x08, 0x8E);
    idt_set_gate(0x21, (uint32_t)irq1,  0x08, 0x8E);
    idt_set_gate(0x22, (uint32_t)irq2,  0x08, 0x8E);
    idt_set_gate(0x23, (uint32_t)irq3,  0x08, 0x8E);
    idt_set_gate(0x24, (uint32_t)irq4,  0x08, 0x8E);
    idt_set_gate(0x25, (uint32_t)irq5,  0x08, 0x8E);
    idt_set_gate(0x26, (uint32_t)irq6,  0x08, 0x8E);
    idt_set_gate(0x27, (uint32_t)irq7,  0x08, 0x8E);
    idt_set_gate(0x28, (uint32_t)irq8,  0x08, 0x8E);
    idt_set_gate(0x29, (uint32_t)irq9,  0x08, 0x8E);
    idt_set_gate(0x2A, (uint32_t)irq10, 0x08, 0x8E);
    idt_set_gate(0x2B, (uint32_t)irq11, 0x08, 0x8E);
    idt_set_gate(0x2C, (uint32_t)irq12, 0x08, 0x8E);
    idt_set_gate(0x2D, (uint32_t)irq13, 0x08, 0x8E);
    idt_set_gate(0x2E, (uint32_t)irq14, 0x08, 0x8E);
    idt_set_gate(0x2F, (uint32_t)irq15, 0x08, 0x8E);

    loadIDT(&idt_ptr);
}

// Called from isr_common_stub in idt.asm when an interrupt fires
void isr_handler(uint32_t int_no, uint32_t err_code) {
    char* messages[] = {
        "ISR0: Division By Zero",
        "ISR1: Debug",
        "ISR2: Non-Maskable Interrupt",
        "ISR3: Breakpoint",
        "ISR4: Overflow",
        "ISR5: Bound Range Exceeded"
    };
    print_string(messages[int_no], 0x04); // print in red
}