#ifndef UTIL_H
#define UTIL_H

#include <libc/stdint.h>

// ==============================
// Utility functions
//
// Provides low-level helpers for
// port I/O and error handling.
// ==============================

//void memset(void *dest, char val, uint32_t count);
uint8_t inPortB(uint16_t port);                 // Read a byte from an I/O port
void outPortB(uint16_t port, uint8_t value);    // Write a byte to an I/O port
void panic(const char* message);                // Halt the system with an error message

#endif
