#include "libc/stdint.h" // Bruk den lokale libc-fila
#include "gdt.h"
#include "terminal.h"

// Definerer multiboot-strukturen slik at kompilatoren vet hva det er
struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    void* first;
};

void main(uint32_t magic, struct multiboot_info* mb_info) {
    // 1. Initialiser GDT (Gjør at vi kan bruke minne riktig)
    gdt_init();

    // 2. Initialiser skjermen
    terminal_init();

    // 3. Skriv ut tekst
    terminal_write("Hello World!\n");
    terminal_write("GDT is now active.\n");
    terminal_write("OSDev 2026 - Gruppe 12\n");

    // 4. Evig løkke
    while (1) {
        __asm__ volatile("hlt");
    }
}