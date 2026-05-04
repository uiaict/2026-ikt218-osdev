#include <libc/stdio.h>

#include <libc/stdarg.h>
#include <libc/stdbool.h>
#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/string.h>
#include <terminal.h>

static int print_unsigned(unsigned int value, unsigned int base)
{
    char buffer[32];
    int length = 0;
    int written = 0;
    const char* digits = "0123456789abcdef";

    if (value == 0) {
        putchar('0');
        return 1;
    }

    while (value > 0) {
        buffer[length++] = digits[value % base];
        value /= base;
    }

    while (length > 0) {
        putchar(buffer[--length]);
        written++;
    }

    return written;
}

static int print_signed(int value)
{
    if (value < 0) {
        unsigned int magnitude = (unsigned int)(-(value + 1)) + 1;
        putchar('-');
        return 1 + print_unsigned(magnitude, 10);
    }

    return print_unsigned((unsigned int)value, 10);
}

int putchar(int ic)
{
    char c = (char)ic;
    terminal_putchar(c);
    return ic;
}

bool print(const char* data, size_t length)
{
    terminal_write(data, length);
    return true;
}

int printf(const char* __restrict__ format, ...)
{
    va_list parameters;
    int written = 0;

    va_start(parameters, format);

    while (*format != '\0') {
        if (*format != '%') {
            putchar(*format++);
            written++;
            continue;
        }

        format++;
        if (*format == '\0') {
            putchar('%');
            written++;
            break;
        }

        switch (*format) {
        case '%':
            putchar('%');
            written++;
            break;
        case 'c':
            putchar((char)va_arg(parameters, int));
            written++;
            break;
        case 's': {
            const char* value = va_arg(parameters, const char*);
            if (value == NULL) {
                value = "(null)";
            }
            size_t length = strlen(value);
            print(value, length);
            written += (int)length;
            break;
        }
        case 'd':
        case 'i':
            written += print_signed(va_arg(parameters, int));
            break;
        case 'u':
            written += print_unsigned(va_arg(parameters, unsigned int), 10);
            break;
        case 'x':
            written += print_unsigned(va_arg(parameters, unsigned int), 16);
            break;
        default:
            putchar('%');
            putchar(*format);
            written += 2;
            break;
        }

        format++;
    }

    va_end(parameters);
    return written;
}
