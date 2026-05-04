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
void terminal_putchar(uint8 c, uint8 color, int x, int y);

/**
 * @brief Prints a null-terminated string to the screen with automatic wrapping.
 * * @param str    Pointer to the string to be printed
 * @param color  The color attribute for the text
 * @param x      Starting column
 * @param y      Starting row
 */
void terminal_write(const char* str, uint8 color, int x, int y);
void terminal_setchar(uint8 c, int x, int y);
void terminal_setcharfg(uint8 c, uint8 color_fg, int x, int y);
void terminal_setcolor(uint8 color, int x, int y);
void terminal_setbgcolor(uint8 color, int x, int y);
uint8 terminal_getbgcolor(int x, int y);

// Clear terminal with a uint8 color attribute @param color
void terminal_clear(uint8 color);

#endif // TERMINAL_H