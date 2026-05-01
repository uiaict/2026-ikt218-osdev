#include <libc/stdio.h>
#include <libc/stdint.h>
#include <libc/stdarg.h>
#include <libc/stdbool.h>
#include <libc/stddef.h>
#include "../include/print.h"

// putchar writes a single character to the VGA buffer via print_string
int putchar(int ic) {
    char c = (char)ic;
    if (c == '\n') {
        print_newline();
    } else {
        char buf[2] = {c, '\0'};
        print_string(buf, 0x07);
    }
    return ic;
}

bool print(const char* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        putchar((int)data[i]);
    }
    return true;
}

// Helper: print an unsigned integer in a given base (10 or 16)
static void print_uint(uint32_t value, uint32_t base) {
    char buf[32];
    int i = 0;
    if (value == 0) { putchar('0'); return; }
    while (value > 0) {
        int digit = value % base;
        buf[i++] = digit < 10 ? '0' + digit : 'a' + digit - 10;
        value /= base;
    }
    // buf is reversed
    while (i > 0) putchar(buf[--i]);
}

static void print_int(int32_t value) {
    if (value < 0) { putchar('-'); print_uint((uint32_t)(-value), 10); }
    else print_uint((uint32_t)value, 10);
}

int printf(const char* __restrict__ format, ...) {
    va_list args;
    va_start(args, format);

    for (int i = 0; format[i]; i++) {
        if (format[i] != '%') { putchar(format[i]); continue; }
        i++;
        switch (format[i]) {
            case 'd': print_int(va_arg(args, int));          break;
            case 'u': print_uint(va_arg(args, uint32_t), 10); break;
            case 'x': print_uint(va_arg(args, uint32_t), 16); break;
            case 's': { 
                const char* s = va_arg(args, const char*);
                while (*s) putchar(*s++);
                break; 
            }
            case 'c': putchar(va_arg(args, int));             break;
            case '%': putchar('%');                            break;
            default:  putchar('%'); putchar(format[i]);       break;
        }
    }

    va_end(args);
    return 0;
}

void panic(const char* msg) {
    printf("PANIC: %s\n", msg);
    asm volatile ("cli; hlt");
}