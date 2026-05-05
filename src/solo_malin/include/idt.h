#ifndef IDT_H
#define IDT_H

#include <libc/stdint.h>  

// ==============================
// Interrupt Descriptor Table (IDT)
//
// Defines the structures and
// functions used to handle interrupts.
//
// The IDT contains entries that
// point to interrupt handler functions.
// ==============================

// Number of entries in the IDT
#define IDT_ENTRIES 256

// IDT entry structure
struct idt_entry {
    uint16_t base_low;      // Lower 16 bits of handler address
    uint16_t sel;           // Kenel segment selector
    uint8_t always0;        // Always set to 0
    uint8_t flags;          // Flags
    uint16_t base_high;     // Upper 16 bits of handler address 
} __attribute__((packed));


// Structure used when loading the IDT 
struct idt_ptr {
    uint16_t limit;     
    uint32_t base;      
} __attribute__((packed));

// Inititalize and set up the IDT
void idt_init(void);

// Set an IDT entry
void setIdtGate(uint32_t num, uint32_t base, uint16_t sel, uint8_t flags);

#endif