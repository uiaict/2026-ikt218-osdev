#include <libc/stdbool.h>
#include <libc/stdint.h>
#include <libc/stdarg.h>
#include <libc/string.h>
#include <libc/stdio.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile uint16_t *)0xB8000)

static int     cursor_x  = 0;
static int     cursor_y  = 0;
static uint8_t cur_color = VGA_COLOR_BWHITE;

static void scroll(void) {
    if (cursor_y < VGA_HEIGHT)
        return;
    for (int y = 0; y < VGA_HEIGHT - 1; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            VGA_MEMORY[y * VGA_WIDTH + x] = VGA_MEMORY[(y + 1) * VGA_WIDTH + x];
    uint16_t blank = ((uint16_t)cur_color << 8) | ' ';
    for (int x = 0; x < VGA_WIDTH; x++)
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    cursor_y = VGA_HEIGHT - 1;
}

void terminal_clear(void) {
    uint16_t blank = ((uint16_t)cur_color << 8) | ' ';
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = blank;
    cursor_x = 0;
    cursor_y = 0;
}

void terminal_set_color(unsigned char color) {
    cur_color = color;
}

int putchar(int ic) {
    char c = (char)ic;
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] = ((uint16_t)cur_color << 8) | ' ';
        }
    } else {
        VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] = ((uint16_t)cur_color << 8) | (uint8_t)c;
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    scroll();
    return (unsigned char)c;
}

bool print(const char *data, size_t length) {
    for (size_t i = 0; i < length; i++)
        putchar(data[i]);
    return true;
}

static void print_uint(uint32_t n, int base) {
    char buf[32];
    int i = 0;
    if (n == 0) { putchar('0'); return; }
    while (n) {
        int d = (int)(n % (uint32_t)base);
        buf[i++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
        n /= (uint32_t)base;
    }
    while (i > 0) putchar(buf[--i]);
}

static void print_int(int32_t n) {
    if (n < 0) { putchar('-'); print_uint((uint32_t)(-n), 10); }
    else        { print_uint((uint32_t)n, 10); }
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int written = 0;
    for (int i = 0; fmt[i]; i++) {
        if (fmt[i] != '%') { putchar(fmt[i]); written++; continue; }
        i++;
        switch (fmt[i]) {
            case 'c': putchar((char)va_arg(args, int)); written++; break;
            case 's': { const char *s = va_arg(args, const char *); while (*s) { putchar(*s++); written++; } break; }
            case 'd': print_int(va_arg(args, int32_t)); break;
            case 'u': print_uint(va_arg(args, uint32_t), 10); break;
            case 'x': case 'X': print_uint(va_arg(args, uint32_t), 16); break;
            case '%': putchar('%'); written++; break;
            default: putchar('%'); putchar(fmt[i]); written += 2; break;
        }
    }
    va_end(args);
    return written;
}
