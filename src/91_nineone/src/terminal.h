#ifndef TERMINAL_H
#define TERMINAL_H

#include "libc/stdint.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

/**
 * @brief Prints a single character to a specific location in the VGA buffer.
 * * @param c      The character to display (ASCII/CP437)
 * @param color  Color attribute (background << 4 | foreground)
 * @param x      Column position (0-79)
 * @param y      Row position (0-24)
 */
void terminal_putchar(char c, uint8 color, int x, int y);

/**
 * @brief Prints a null-terminated string to the screen with automatic wrapping.
 * * @param str    Pointer to the string to be printed
 * @param color  The color attribute for the text
 * @param x      Starting column
 * @param y      Starting row
 */
void terminal_write(const char* str, uint8 color, int x, int y);

#endif // TERMINAL_H