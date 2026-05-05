#include "irq.h"
#include "terminal.h"
#include "libc/stdint.h"

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

void irq_init() {
    // Masker ALLE interrupts først
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
    
    // ICW1
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);
    
    // ICW2 - offsets
    outb(PIC1_DATA, 32);   // IRQ0-7 → interrupts 32-39
    outb(PIC2_DATA, 40);   // IRQ8-15 → interrupts 40-47
    
    // ICW3 - cascade
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    
    // ICW4
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // Masker alle UNNTATT keyboard (IRQ1 = bit 1)
    // Bit 1 = 0 (enabled), rest = 1 (masked)
    // 0xFD = 1111 1101
    outb(PIC1_DATA, 0xFD);
    outb(PIC2_DATA, 0xFF);
    
    terminal_write("[IRQ] Init done\n");
}