#include <libc/stdint.h>
#include <libc/stdarg.h>
#include "screen.h"

#define VGA ((unsigned short*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static unsigned int pos = 0;

void scroll() {
    unsigned short blank = 0x0F00 | ' ';

    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA[(y - 1) * VGA_WIDTH + x] = VGA[y * VGA_WIDTH + x];
        }
    }

    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }

    pos = (VGA_HEIGHT - 1) * VGA_WIDTH;
}

void write_char(char c) {
    if (c == '\n') {
        pos += VGA_WIDTH - (pos % VGA_WIDTH);
    } else {
        VGA[pos++] = 0x0F00 | (unsigned short)c;
    }

    if (pos >= VGA_WIDTH * VGA_HEIGHT){
        scroll();
    }


}

void write_string(const char *s) {
    while (*s) {
        write_char(*s++);
    }
}

void write_dec(uint32_t n) {
    char buf[16];
    int i = 0;

    if (n == 0) {
        write_char('0');
        return;
    }

    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }

    while (i > 0) {
        write_char(buf[--i]);
    }
}

void write_hex(uint32_t n) {
    char hex[] = "0123456789ABCDEF";
    write_string("0x");

    for (int i = 28; i >= 0; i -= 4) {
        write_char(hex[(n >> i) & 0xF]);
    }
}

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