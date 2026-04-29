// gdt.c
#include "gdt.h"

// The table - an array of 3 entries (null, code, data)
struct gdt_entry gdt[3];

// The pointer we will hand to the CPU so it can find the table
struct gdt_pointer gdt_ptr;

// Fills in one row of the GDT table
// The base and limit are split across multiple bytes, so we need to break them apart
static void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    // Base is 32 bits but stored in three separate fields, so we split it
    gdt[index].base_low    = (base & 0xFFFF);         // lower 16 bits of base
    gdt[index].base_middle = (base >> 16) & 0xFF;     // bits 16-23 of base
    gdt[index].base_high   = (base >> 24) & 0xFF;     // bits 24-31 of base

    // Limit is also split across two fields
    gdt[index].limit_low   = (limit & 0xFFFF);        // lower 16 bits of limit
    gdt[index].granularity = ((limit >> 16) & 0x0F)   // upper 4 bits of limit
                           | (granularity & 0xF0);    // combined with flags in upper 4 bits

    // Access byte defines permissions (executable, readable, etc.)
    gdt[index].access      = access;
}

void gdt_init() {
    // Tell the pointer how big the table is and where it lives in memory
    gdt_ptr.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_ptr.base  = (uint32_t)&gdt;

    // Entry 0: always null, required by the CPU
    gdt_set_entry(0, 0, 0x00000000, 0x00, 0x00);

    // Entry 1: code segment, covers all 4GB, kernel privilege, executable
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Entry 2: data segment, covers all 4GB, kernel privilege, writable
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    loadGDT(&gdt_ptr); // load address of gdtptr
    reloadSegments();
}