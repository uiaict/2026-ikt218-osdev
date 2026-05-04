#include "../include/irq.h"
#include "../include/pic.h"
#include <../include/libc/stdint.h>
#include "../include/vga.h"
#include "../include/keyboard.h"
#include "../include/pit.h" 

// This file handles irq implementation

// Functions from other modules that we need here
extern void set_idt_entry_public(int n, uint32_t handler);
extern void keyboard_handler(int irq);
extern void pit_irq_handler(void);

extern uint8_t inb_port(uint16_t port);
extern void outb_port(uint16_t port, uint8_t val);

// The 8259 PIC is remapped to start at IRQ_BASE (32) so hardware IRQs don't
// collide with CPU exception vectors 0-31 which are reserved by the CPU
#define IRQ_BASE 32

// Assembly stubs defined in irq.asm - one per hardware IRQ line (0-15)
extern void irq0(); extern void irq1(); extern void irq2(); extern void irq3();
extern void irq4(); extern void irq5(); extern void irq6(); extern void irq7();
extern void irq8(); extern void irq9(); extern void irq10(); extern void irq11();
extern void irq12(); extern void irq13(); extern void irq14(); extern void irq15();

void irq_install() {
    // Remap the PIC 
    pic_remap(IRQ_BASE, IRQ_BASE + 8);

    set_idt_entry_public(IRQ_BASE + 0, (uint32_t)irq0);
    set_idt_entry_public(IRQ_BASE + 1, (uint32_t)irq1);
    set_idt_entry_public(IRQ_BASE + 2, (uint32_t)irq2);
    set_idt_entry_public(IRQ_BASE + 3, (uint32_t)irq3);
    set_idt_entry_public(IRQ_BASE + 4, (uint32_t)irq4);
    set_idt_entry_public(IRQ_BASE + 5, (uint32_t)irq5);
    set_idt_entry_public(IRQ_BASE + 6, (uint32_t)irq6);
    set_idt_entry_public(IRQ_BASE + 7, (uint32_t)irq7);
    set_idt_entry_public(IRQ_BASE + 8, (uint32_t)irq8);
    set_idt_entry_public(IRQ_BASE + 9, (uint32_t)irq9);
    set_idt_entry_public(IRQ_BASE +10, (uint32_t)irq10);
    set_idt_entry_public(IRQ_BASE +11, (uint32_t)irq11);
    set_idt_entry_public(IRQ_BASE +12, (uint32_t)irq12);
    set_idt_entry_public(IRQ_BASE +13, (uint32_t)irq13);
    set_idt_entry_public(IRQ_BASE +14, (uint32_t)irq14);
    set_idt_entry_public(IRQ_BASE +15, (uint32_t)irq15);

    // for inb and outb
    extern uint8_t inb_port(uint16_t port);
    extern void outb_port(uint16_t port, uint8_t val);
  
}

 void irq_handler_c(int irq) {
    // Check for spurious IRQ7
    if (irq == 7) {
        // Read PIC In-Service Register to verify it's real
        outb_port(0x20, 0x0B);          // OCW3: read ISR
        uint8_t isr = inb_port(0x20);
        if (!(isr & 0x80)) return; // Spurious — do NOT send EOI
    }
    // Check for spurious IRQ15
    if (irq == 15) {
        outb_port(0xA0, 0x0B);
        uint8_t isr = inb_port(0xA0);
        if (!(isr & 0x80)) {
            outb_port(0x20, 0x20); // Send EOI to master only
            return;
        }
    }

    if (irq == 0) {
        pit_irq_handler();
    } else if (irq == 1) {
        keyboard_handler(irq);
    }

    pic_send_eoi(irq);
} 
