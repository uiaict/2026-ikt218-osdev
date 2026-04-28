#include "types.h"

struct gdt_entry_struct {
    uint16 limit_low;           // Limit (0-15)
    uint16 base_low;            // Base (0-15)
    uint8  base_middle;         // Base (16-23)
    uint8  access;              // Access flags
    uint8  granularity;         // Limit (16-19) + Granularity
    uint8  base_high;           // Base (24-31)
} __attribute__((packed));

typedef struct gdt_entry_struct GdtEntry;

// Denne strukturen sendes til CPU-en (instruksjonen LGDT)
struct gdt_ptr_struct {
    uint16 limit;               // GDT size
    uint32 base;                // Adressen til den første GDT-oppføringen
} __attribute__((packed));

typedef struct gdt_ptr_struct GdtPtr;

// Vi definerer 3 entries: NULL, Code og Data
GdtEntry gdt_entries[3];
GdtPtr   gdt_ptr;

// Denne funksjonen defineres i Assembly-filen din (gdt_flush.asm)
extern void gdt_flush(uint32);

// Funksjon for å sette opp en enkelt gate i tabellen
static void gdt_set_gate(int32 num, uint32 base, uint32 limit, uint8 access, uint8 gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

// Hovedfunksjonen som kalles fra kernel_main
void init_gdt() {
    // Sett opp GDT-pointeren
    gdt_ptr.limit = (sizeof(GdtEntry) * 3) - 1;
    gdt_ptr.base  = (uint32)&gdt_entries;

    // 1. NULL-segmentet (Påkrevd av CPU-en, skal være bare nuller)
    gdt_set_gate(0, 0, 0, 0, 0);

    // 2. Code Segment: Base 0, Limit 4GB, Tilgang 0x9A, Granularity 0xCF
    // 0x9A betyr: Present, Ring 0, Executable, Readable
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // 3. Data Segment: Base 0, Limit 4GB, Tilgang 0x92, Granularity 0xCF
    // 0x92 betyr: Present, Ring 0, Writable
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Fortell CPU-en om den nye tabellen via Assembly-funksjonen
    gdt_flush((uint32)&gdt_ptr);
}