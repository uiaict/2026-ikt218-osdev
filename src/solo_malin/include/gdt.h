#ifndef GDT_H
#define GDT_H

#include <libc/stdint.h>    // Standard integer types

// Number of entries in the Global Descriptor Tabe, GDT
#define GDT_ENTRIES 5

// Structure representing a single GDT entry (segment description)
struct gdt_entry {
    uint16_t limit_low;     // Lower 16 bits of segment limit
    uint16_t base_low;      // Lower 16 bits of base address
    uint8_t base_middle;    // Nest 8 bits of base address
    uint8_t access;         // Access flags (type, privilage level, present bit)
    uint8_t granularity;    // Granularity, and upper 4 bits of limit
    uint8_t base_high;      // Highest 8 bits of base address
} __attribute__((packed));  // Prevent compiler padding 

// Structure used to load the GDT (GDTR register format)
struct gdt_ptr {
    uint16_t limit;     // Size of the GDT (size -1)
    uint32_t base;      // Address of the first GDT entry
} __attribute__((packed));


void gdt_init();    // Initialize GDT
void gdt_load(struct gdt_ptr * gdt_ptr);    // Load GDT
// Set up a individual GDT entry
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

//Assembly function to flush/load the new GDT 
extern void gdt_flush(uint32_t gdt_ptr);

#endif