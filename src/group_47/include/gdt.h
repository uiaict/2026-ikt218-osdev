#ifndef GDT_H
#define GDT_H

#include "libc/stdint.h"

// En struktur som beskriver en GDT-oppføring
struct gdt_entry_struct {
    uint16_t limit_low;     // De laveste 16 bitene av limit
    uint16_t base_low;      // De laveste 16 bitene av base
    uint8_t  base_middle;   // De neste 8 bitene av base
    uint8_t  access;        // Tilgangsrettigheter (hvilken ring, Code/Data osv)
    uint8_t  granularity;   // Flagg for størrelse og de siste 4 bitene av limit
    uint8_t  base_high;     // De siste 8 bitene av base
} __attribute__((packed)); // "__attribute__((packed))" stopper C fra å legge til ekstra mellomrom

typedef struct gdt_entry_struct gdt_entry_t;

// En struktur som beskriver GDT-pekeren (GDTR) som vi sender til CPU-en
struct gdt_ptr_struct {
    uint16_t limit;               // Størrelsen på GDT-tabellen minus 1
    uint32_t base;                // Adressen til den første gdt_entry_t
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

// Funksjon for å sette opp GDT - denne kaller du fra kernel.c
void init_gdt();

#endif