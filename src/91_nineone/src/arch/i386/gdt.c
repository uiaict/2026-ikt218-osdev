#include "arch/i386/gdt.h"
#include "libc/stdint.h"

struct gdt_entry_struct {    // Must split fields up like this to support ancient 1970s Intel CPU architecture
    uint16 limit_low;        // Limit (0-15)
    uint16 base_low;         // Base (0-15)
    uint8  base_middle;      // Base (16-23)
    uint8  access;           // Access flags
    uint8  granularity;      // Limit (16-19) + Granularity
    uint8  base_high;        // Base (24-31)
} __attribute__((packed));

typedef struct gdt_entry_struct GdtEntry;

struct gdt_ptr_struct {
    uint16 limit;            // GDT size
    uint32 base;             // Where the GDT starts in memory
} __attribute__((packed));

typedef struct gdt_ptr_struct GdtPtr;

GdtEntry gdt_entries[3]; // Will have 3 entries: NULL, Code, Data
GdtPtr   gdt_ptr;

extern void gdt_flush(uint32); // implemented in gdt_flush.asm

// Function for converting entry parameters into Intel's 1970s GDT format
static void gdt_set_gate(int32 num, uint32 base, uint32 limit, uint8 access, uint8 gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

// Main function called from kernel_main
void init_gdt() {
    // Set up the GDT pointer
    gdt_ptr.limit = (sizeof(GdtEntry) * 3) - 1;
    gdt_ptr.base  = (uint32)&gdt_entries;

    // 1. NULL-segment
    gdt_set_gate(0, 0, 0, 0, 0);

    // 2. Code Segment: Base 0, Limit 4GB, Access 0x9A, Granularity 0xCF
    // 0x9A: Present, Ring 0, Executable, Readable
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // 3. Data Segment: Base 0, Limit 4GB, Access 0x92, Granularity 0xCF
    // 0x92: Present, Ring 0, Writable
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Tell CPU about the new table via Assembly function
    gdt_flush((uint32)&gdt_ptr);
}