#include <libc/stdint.h>
#include <libc/stdarg.h>
#include "screen.h"

#define VGA ((unsigned short*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Current cursor position (as linear index into VGA buffer)
static unsigned int pos = 0;

// ==============================
// Scrolling
//
// Scroll screen up by one line
// when the bottom is reached.
// ==============================

void scroll() {
    unsigned short blank = 0x0F00 | ' ';        // White-on-black space

    // Move each line up one row
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA[(y - 1) * VGA_WIDTH + x] = VGA[y * VGA_WIDTH + x];
        }
    }

    // Clear last line
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }

    // Place cursor at start of last line
    pos = (VGA_HEIGHT - 1) * VGA_WIDTH;
}


// ==============================
// Basic character output
// ==============================

void write_char(char c) {
    if (c == '\n') {
        // Move to start of next line
        pos += VGA_WIDTH - (pos % VGA_WIDTH);
    } else {
        VGA[pos++] = 0x0F00 | (unsigned short)c;    // White-on-black char
    }

    // Scroll if we go past bottom of screen
    if (pos >= VGA_WIDTH * VGA_HEIGHT){
        scroll();
    }
}

// Write a null-terminated string
void write_string(const char *s) {
    while (*s) {
        write_char(*s++);
    }
}

// ==============================
// Number output helpers
// ==============================

void write_dec(uint32_t n) {
    char buf[16];
    int i = 0;

    if (n == 0) {
        write_char('0');
        return;
    }

    // Build digits in reverse
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }

    // Print in correct order
    while (i > 0) {
        write_char(buf[--i]);
    }
}

void write_hex(uint32_t n) {
    char hex[] = "0123456789ABCDEF";
    write_string("0x");

    // Print 8 hex digits (32 bits)
    for (int i = 28; i >= 0; i -= 4) {
        write_char(hex[(n >> i) & 0xF]);
    }
}

// ==============================
// kprintf (minimal printf)
// Supports: %d, %x, %s, %%
// ==============================

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;

            if (*fmt == 'd') {
                uint32_t val = va_arg(args, uint32_t);
                write_dec(val);
            } else if (*fmt == 'x') {
                uint32_t val = va_arg(args, uint32_t);
                write_hex(val);
            } else if (*fmt == 's') {
                char* str = va_arg(args, char*);
                write_string(str);
            } else if (*fmt == '%') {
                write_char('%');
            } else {
                // Unknown specifier: print it literally
                write_char('%');
                write_char(*fmt);
            }
        } else {
            write_char(*fmt);
        }

        fmt++;
    }

    va_end(args);
}

// ==============================
// Clear entire screen
// ==============================

void clearTerminal(void) {
    unsigned short blank = 0x0F00 | ' ';

    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA[y * VGA_WIDTH + x] = blank;
        }
    }

    pos = 0;    // Reset cursor to top-left
}