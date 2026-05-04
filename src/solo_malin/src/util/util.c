#include "util.h"
#include "screen.h"

// ==============================
// Low-level utilities
//
// Port I/O helpers and a simple
// kernel panic function.
// ==============================

// Write a byte to an I/O port
void outPortB(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a" (value), "Nd" (port));

}

// Read a byte from an I/O port
uint8_t inPortB(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Print panic message and halt the system
void panic(const char* message)
{
    write_string("KERNEL PANIC: ");
    write_string(message);
    write_string("\n");

    // Disable interrupts and halt forever
    for (;;) {
        __asm__ __volatile__("cli; hlt");
    }
}