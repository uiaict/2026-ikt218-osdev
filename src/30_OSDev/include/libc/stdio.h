#ifndef UIAOS_STDIO_H
#define UIAOS_STDIO_H

/* Use the project's own headers when -nostdinc is enabled */
#include <stddef.h>
#include "stdbool.h"

/* Simple prototypes for your freestanding I/O */
int putchar(int ic);

/* print: write 'length' bytes from 'data' to terminal/tty */
bool print(const char* data, size_t length);

/* printf: only include this if you actually have an implementation.
   If not, comment out this declaration and remove calls in your code. */
int printf(const char* __restrict__ format, ...);

#endif /* UIAOS_STDIO_H */