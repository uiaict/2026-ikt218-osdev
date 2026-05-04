#include "gdt.h"

// ==============================
// Global Descriptor Table (GDT)
//
// Builds the GDT entries for
// kernel/user code and data
// segments, then loads/flushed.
// ==============================


struct gdt_entry gdt[GDT_ENTRIES];  // Array holding all GDT entires
struct gdt_ptr gdt_ptr;     // Pointer structure used to load the GDT

// Initialize and install the GDT
void gdt_init() {

    // Set the GDT limit
    gdt_ptr.limit = sizeof(struct gdt_entry)*GDT_ENTRIES -1;
    gdt_ptr.base = (uint32_t) &gdt;

    // Setup GDT entries
    gdt_set_gate(0, 0, 0, 0, 0);    // Null segment 
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);     // Kernel Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);     // Kernel Data segment
    
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);     // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);     // User mode data segment
    
    // Load the GDT (lgdt)
    gdt_load(&gdt_ptr);     // Load the GDT using lgdt instruction

    // Flush segment registers to use new GDT
    gdt_flush((uint32_t)&gdt_ptr);  // Flush/upate segment registers
}

// Load the GDT using lgdt
void gdt_load(struct gdt_ptr *gdt_ptr) {
    asm volatile("lgdt %0" :: "m" (*gdt_ptr));
}

// Set the value of one GDT entry
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
 
    // Split base address into 3 parts and store them
    gdt[num].base_low = (base & 0xFFFF);            // Lower 16 bits
    gdt[num].base_middle = (base >> 16) & 0xFF;     // Middle 8 bits
    gdt[num].base_high = (base >> 24) & 0xFF;       // Highest 8 bits

     // Split limit into lower 16 bits and upper 4 bits
    gdt[num].limit_low = (limit & 0xFFFF);          // Split limit into lower 16 bits
    gdt[num].granularity = (limit >> 16) & 0x0F;    // Store upper 4 bits of limit in granularity field

    gdt[num].granularity |= gran & 0xF0;            // Combine granularity flags (upper 4 bits) with existing limit bits
    gdt[num].access = access;                       // Set access flags 

}