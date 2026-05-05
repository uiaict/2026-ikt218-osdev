#include "ports.h"

uint8_t port_byte_in(uint16_t port) {
    uint8_t result;
    // 'volatile' prevents the compiler from optimizing this out
    __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}

void port_byte_out(uint16_t port, uint8_t data) {
    // 'volatile' prevents the compiler from optimizing this out
    __asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}