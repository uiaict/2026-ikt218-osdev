#include <libc/stdarg.h>
#include <libc/stdint.h>
#include <libc/stddef.h>
#include <libc/stdbool.h>
#include <libc/stdio.h>

static volatile char* const VGA_TEXT_BUFFER = (volatile char*)0xB8000;
static size_t terminal_column;
static const char terminal_color = 0x07;

static void terminal_put_char(char c) {
    if (c == '\n') {
        terminal_column = 0;
        return;
    }

    const size_t offset = terminal_column * 2U;
    VGA_TEXT_BUFFER[offset] = c;
    VGA_TEXT_BUFFER[offset + 1U] = terminal_color;
    terminal_column++;
}

int putchar(int ic) {
    terminal_put_char((char)ic);
    return ic;
}

bool print(const char* data, size_t length) {
    if (data == NULL) {
        return false;
    }

    for (size_t i = 0; i < length; i++) {
        terminal_put_char(data[i]);
    }

    return true;
}

int printf(const char* __restrict__ format, ...) {
    size_t written = 0;

    if (format == NULL) {
        return 0;
    }

    va_list args;
    va_start(args, format);
    va_end(args);

    while (format[written] != '\0') {
        terminal_put_char(format[written]);
        written++;
    }

    return (int)written;
}
