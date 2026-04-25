#ifndef TERMINAL_H
#define TERMINAL_H

#include "libc/stddef.h"
#include "libc/stdint.h"

// Initialiser terminal-skjermen
void terminal_initialize(void);

// Skriv en karakter til terminalen
void terminal_putchar(char c);

// Skriv en string til terminalen (printf)
void printf(const char* data);

#endif