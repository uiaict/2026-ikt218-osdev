#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "util.h"
#include "memory.h"


// Global IDT table and pointer
struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr idt_ptr;

// Assembly function to load the IDT (defined in idt.asm)
extern void idt_flush(uint32_t);

// Set a single IDT entry (gate)
void setIdtGate(uint32_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;              // Handler address (low 16 bits)
    idt[num].base_high = (base >> 16) & 0xFFFF;     // Handler address (high 16 bits)
    idt[num].sel = sel;                             // Code segment selector
    idt[num].always0 = 0;                           // Must be zero
    idt[num].flags = flags;                         // Type and attributes (e.g., 0x8E = 32‑bit interrupt gate)
}

// Initialize and load the IDT
void idt_init(void) {

    // Set IDT pointer (size and base address)
    idt_ptr.limit = sizeof(idt) -1;
    idt_ptr.base = (uint32_t)&idt;

    // Clear the entire IDT
    memset(&idt, 0, sizeof(idt));

    // -------------------------------
    // Remap the PIC (Programmable Interrupt Controller)
    // -------------------------------

    outPortB(0x20, 0x11); // Start initialization of master PIC
    outPortB(0xA0, 0x11); // Start initialization of slave PIC

    outPortB(0x21, 0x20); // Set master PIC vector offset to 32 (0x20)
    outPortB(0xA1, 0x28); // Set slave PIC vector offset to 40 (0x28)

    outPortB(0x21, 0x04); // Tell master PIC that slave is at IRQ2
    outPortB(0xA1, 0x02); // Tell slave PIC its cascade identity

    outPortB(0x21, 0x01); // Set PIC to 8086/88 mode (MCS-80/85 mode)
    outPortB(0xA1, 0x01);

    outPortB(0x21, 0x0);  // Clear interrupt masks (enable all IRQs)
    outPortB(0xA1, 0x0);


    // -------------------------------
    // Set CPU exception handlers (ISRs 0–31)
    // -------------------------------
    // 0x8E = present, ring 0, 32-bit interrupt gate
    // 0x08 = kernel code segment selector

    setIdtGate(0, (uint32_t)isr0, 0x08, 0x8E);
    setIdtGate(1, (uint32_t)isr1, 0x08, 0x8E);
    setIdtGate(2, (uint32_t)isr2, 0x08, 0x8E);
    setIdtGate(3, (uint32_t)isr3, 0x08, 0x8E);
    setIdtGate(4, (uint32_t)isr4, 0x08, 0x8E);
    setIdtGate(5, (uint32_t)isr5, 0x08, 0x8E);
    setIdtGate(6, (uint32_t)isr6, 0x08, 0x8E);
    setIdtGate(7, (uint32_t)isr7, 0x08, 0x8E);
    setIdtGate(8, (uint32_t)isr8, 0x08, 0x8E);
    setIdtGate(9, (uint32_t)isr9, 0x08, 0x8E);
    setIdtGate(10, (uint32_t)isr10, 0x08, 0x8E);
    setIdtGate(11, (uint32_t)isr11, 0x08, 0x8E);
    setIdtGate(12, (uint32_t)isr12, 0x08, 0x8E);
    setIdtGate(13, (uint32_t)isr13, 0x08, 0x8E);
    setIdtGate(14, (uint32_t)isr14, 0x08, 0x8E);
    setIdtGate(15, (uint32_t)isr15, 0x08, 0x8E);
    setIdtGate(16, (uint32_t)isr16, 0x08, 0x8E);
    setIdtGate(17, (uint32_t)isr17, 0x08, 0x8E);
    setIdtGate(18, (uint32_t)isr18, 0x08, 0x8E);
    setIdtGate(19, (uint32_t)isr19, 0x08, 0x8E);
    setIdtGate(20, (uint32_t)isr20, 0x08, 0x8E);
    setIdtGate(21, (uint32_t)isr21, 0x08, 0x8E);
    setIdtGate(22, (uint32_t)isr22, 0x08, 0x8E);
    setIdtGate(23, (uint32_t)isr23, 0x08, 0x8E);
    setIdtGate(24, (uint32_t)isr24, 0x08, 0x8E);
    setIdtGate(25, (uint32_t)isr25, 0x08, 0x8E);
    setIdtGate(26, (uint32_t)isr26, 0x08, 0x8E);
    setIdtGate(27, (uint32_t)isr27, 0x08, 0x8E);
    setIdtGate(28, (uint32_t)isr28, 0x08, 0x8E);
    setIdtGate(29, (uint32_t)isr29, 0x08, 0x8E);
    setIdtGate(30, (uint32_t)isr30, 0x08, 0x8E);
    setIdtGate(31, (uint32_t)isr31, 0x08, 0x8E);
    

    // -------------------------------
    // Set hardware interrupt handlers (IRQs 32–47)
    // -------------------------------

    setIdtGate(32, (uint32_t)irq0, 0x08, 0x8E);
    setIdtGate(33, (uint32_t)irq1, 0x08, 0x8E);
    setIdtGate(34, (uint32_t)irq2, 0x08, 0x8E);
    setIdtGate(35, (uint32_t)irq3, 0x08, 0x8E);
    setIdtGate(36, (uint32_t)irq4, 0x08, 0x8E);
    setIdtGate(37, (uint32_t)irq5, 0x08, 0x8E);
    setIdtGate(38, (uint32_t)irq6, 0x08, 0x8E);
    setIdtGate(39, (uint32_t)irq7, 0x08, 0x8E);
    setIdtGate(40, (uint32_t)irq8, 0x08, 0x8E);
    setIdtGate(41, (uint32_t)irq9, 0x08, 0x8E);
    setIdtGate(42, (uint32_t)irq10, 0x08, 0x8E);
    setIdtGate(43, (uint32_t)irq11, 0x08, 0x8E);
    setIdtGate(44, (uint32_t)irq12, 0x08, 0x8E);
    setIdtGate(45, (uint32_t)irq13, 0x08, 0x8E);
    setIdtGate(46, (uint32_t)irq14, 0x08, 0x8E);
    setIdtGate(47, (uint32_t)irq15, 0x08, 0x8E);


    // -------------------------------
    // System call interrupts
    // -------------------------------

    setIdtGate(128, (uint32_t)isr128, 0x08, 0x8E);
    setIdtGate(177, (uint32_t)isr177, 0x08, 0x8E);

       // Load the IDT into the CPU (using assembly instruction lidt)
    idt_flush((uint32_t)&idt_ptr);
}