#include "gdt.h"
#include "libc/stdint.h"

// Informasjon om ett minnesegment
struct gdt_entry {
    uint16_t limit_low;      // Nedre del av størrelse
    uint16_t base_low;       // Nedre del av startadresse
    uint8_t base_middle;     // Midtre del av startadresse
    uint8_t access;          // Tilgang (lese/skrive)
    uint8_t granularity;     // Størrelses-enhet
    uint8_t base_high;       // Øvre del av startadresse
} __attribute__((packed));

// Peker til hele GDT-tabellen
struct gdt_ptr {
    uint16_t limit;          // Størrelse på tabellen
    uint32_t base;           // Adressen til tabellen
} __attribute__((packed));

// Lagre 3 segmenter: Null, Kode, Data
struct gdt_entry gdt[3];
struct gdt_ptr gp;

// Assembler-funksjon som laster GDT inn i CPU-en
extern void gdt_flush(uint32_t);

// Fyll ut ett segment i tabellen
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;
    
    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_init() {
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base  = (uint32_t)&gdt;

    // Null-segment (må alltid være nr. 0)
    gdt_set_gate(0, 0, 0, 0, 0);
    
    // Kode-segment (der programmet ligger)
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Data-segment (der variabler lagres)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Last inn i CPU-en
    gdt_flush((uint32_t)&gp);
}