#include "libc/libs.h"

static uint16_t *const VGA_MEMORY = (uint16_t *)0xB8000; // VGA text mode buffer address
static size_t terminal_row = 0;                          // Current row in the terminal
static size_t terminal_column = 0;                       // Current column in the terminal
static uint8_t terminal_color = 0x0F;                    // Terminal color

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8; // Combine character and color into a single 16-bit value
}

int putchar(int ic) // Writes a character to the terminal
{
    unsigned char c = (unsigned char)ic;

    if (c == '\n')
    {
        terminal_column = 0;
        terminal_row++;
        return ic;
    }

    if (c == '\b')
    {
        if (terminal_column > 0)
        {
            terminal_column--;
        }
        else if (terminal_row > 0)
        {
            terminal_row--;
            terminal_column = 79;
        }
        return ic;
    }

    const size_t index = terminal_row * 80 + terminal_column; // Calculate the index in the VGA memory for the current position
    VGA_MEMORY[index] = vga_entry(c, terminal_color);         // Write the character with the current color to the VGA memory

    terminal_column++;
    if (terminal_column == 80)
    {
        terminal_column = 0;
        terminal_row++;
    }

    return ic;
}

static void print_number(int value)
{
    char buffer[16];
    int i = 0;

    if (value == 0)
    {
        putchar('0');
        return;
    }

    if (value < 0)
    {
        putchar('-');
        value = -value;
    }

    while (value > 0)
    {
        buffer[i++] = '0' + (value % 10); // Store the last digit as a character in the buffer
        value /= 10;
    }

    while (i > 0)
    {
        putchar(buffer[--i]);
    }
}

int printf(const char *format, ...) // A simple implementation of printf that supports %d, %s, and %c format specifiers
{
    va_list args;
    va_start(args, format);

    int written = 0;

    for (size_t i = 0; format[i] != '\0'; i++) // Iterate through each character in the format string
    {

        if (format[i] == '%') // Check for format specifier
        {
            i++;

            if (format[i] == 'd')
            {
                int value = va_arg(args, int);
                print_number(value);
            }
            else if (format[i] == 's')
            {
                char *str = va_arg(args, char *); // Get the string argument and print it character by character
                while (*str)
                {
                    putchar(*str++);
                }
            }
            else if (format[i] == 'c')
            {
                char c = (char)va_arg(args, int);
                putchar(c);
            }
        }
        else
        {
            putchar(format[i]);
        }

        written++;
    }

    va_end(args); // Clean up the variable argument list
    return written;
}