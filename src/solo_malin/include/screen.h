#ifndef SCREEN_H
#define SCREEN_H

#include <libc/stdint.h>
#include <libc/stdarg.h>

// ==============================
// Screen output
//
// Provides functions for writing
// text and formatted output to
// the display.
// ==============================


void scroll(void);                      // Scroll the screen when full
void write_string(const char *s);       // Write a string to the screen
void write_char(char c);                // Write a single character
void write_dec(uint32_t n);             // Write a decimal number
void write_hex(uint32_t n);             // Write a hexadecimal number
void kprintf(const char* fmt, ...);     // Formatted print (like printf)
void clearTerminal(void);               // Clear the terminal screen

#endif