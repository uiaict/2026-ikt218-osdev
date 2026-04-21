#include "libc/stdio.h"
#include "libc/stdarg.h"
#include "libc/stdint.h"
#include "libc/string.h"

static uint16_t* const vga_buffer = (uint16_t*)0xB8000;
static size_t vga_row = 0;
static size_t vga_col = 0;
static const uint8_t vga_color = 0x0F;

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void serial_init(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

static void serial_putchar(char c) {
    while ((inb(0x3F8 + 5) & 0x20) == 0) {
    }
    outb(0x3F8, (uint8_t)c);
}

static void vga_putchar(char c) {
    if (c == '\n') {
        vga_col = 0;
        vga_row++;
        return;
    }

    const size_t index = vga_row * 80 + vga_col;
    vga_buffer[index] = ((uint16_t)vga_color << 8) | (uint8_t)c;

    vga_col++;
    if (vga_col >= 80) {
        vga_col = 0;
        vga_row++;
    }
}

static void write_unsigned(unsigned int value, unsigned int base) {
    char buffer[32];
    size_t i = 0;

    if (value == 0) {
        putchar('0');
        return;
    }

    while (value > 0 && i < sizeof(buffer)) {
        const unsigned int digit = value % base;
        buffer[i++] = (digit < 10) ? (char)('0' + digit) : (char)('a' + digit - 10);
        value /= base;
    }

    while (i > 0) {
        putchar(buffer[--i]);
    }
}

static void write_signed(int value) {
    if (value < 0) {
        putchar('-');
        write_unsigned((unsigned int)(-value), 10);
        return;
    }

    write_unsigned((unsigned int)value, 10);
}

int putchar(int ic) {
    static bool serial_ready = false;
    const char c = (char)ic;

    if (!serial_ready) {
        serial_init();
        serial_ready = true;
    }

    if (c == '\n') {
        serial_putchar('\r');
    }
    serial_putchar(c);
    vga_putchar(c);

    return ic;
}

bool print(const char* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        putchar(data[i]);
    }
    return true;
}

int printf(const char* __restrict__ format, ...) {
    va_list args;
    va_start(args, format);

    int written = 0;
    for (size_t i = 0; format[i] != '\0'; i++) {
        if (format[i] != '%') {
            putchar(format[i]);
            written++;
            continue;
        }

        i++;
        switch (format[i]) {
            case '%':
                putchar('%');
                written++;
                break;
            case 'c': {
                const char c = (char)va_arg(args, int);
                putchar(c);
                written++;
                break;
            }
            case 's': {
                const char* s = va_arg(args, const char*);
                if (s == 0) {
                    s = "(null)";
                }
                const size_t len = strlen(s);
                print(s, len);
                written += (int)len;
                break;
            }
            case 'd': {
                const int n = va_arg(args, int);
                write_signed(n);
                break;
            }
            case 'u': {
                const unsigned int n = va_arg(args, unsigned int);
                write_unsigned(n, 10);
                break;
            }
            case 'x': {
                const unsigned int n = va_arg(args, unsigned int);
                write_unsigned(n, 16);
                break;
            }
            default:
                putchar('%');
                putchar(format[i]);
                written += 2;
                break;
        }
    }

    va_end(args);
    return written;
}
