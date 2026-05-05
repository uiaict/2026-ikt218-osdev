/**
 * terminal.h
 * VGA Text Mode Terminal Driver
 * 
 * This file provides a simple interface for interacting with the VGA text mode buffer (0xB8000).
 * It supports basic text output, scrolling, color management, and hardware cursor control.
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include <libc/stdint.h>
#include <libc/stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Standard VGA color palette.

 * 
 * These constants represent the 16 colors available in VGA text mode.
 */
typedef enum {
    VGA_COLOR_BLACK         = 0,
    VGA_COLOR_BLUE          = 1,
    VGA_COLOR_GREEN         = 2,
    VGA_COLOR_CYAN          = 3,
    VGA_COLOR_RED           = 4,
    VGA_COLOR_MAGENTA       = 5,
    VGA_COLOR_BROWN         = 6,
    VGA_COLOR_LIGHT_GREY    = 7,
    VGA_COLOR_DARK_GREY     = 8,
    VGA_COLOR_LIGHT_BLUE    = 9,
    VGA_COLOR_LIGHT_GREEN   = 10,
    VGA_COLOR_LIGHT_CYAN    = 11,
    VGA_COLOR_LIGHT_RED     = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW        = 14,
    VGA_COLOR_WHITE         = 15,
} vga_colors;

/**
 * Initializes the terminal, clears the screen, and resets the cursor.
 */
void terminal_initialize(void);

/**
 * Clears the entire screen with the current background color.
 */
void terminal_clear(void);

/**
 * Sets the foreground and background colors for subsequent text output.
 * 
 * @param fg Foreground color (0-15).
 * @param bg Background color (0-15).
 */
void terminal_set_color(uint8_t fg, uint8_t bg);

/**
 * Places a character at a specific location on the screen with a specific color.
 * 
 * @param c The character to write.
 * @param color The VGA color attribute.
 * @param x Column (0 to VGA_WIDTH-1).
 * @param y Row (0 to VGA_HEIGHT-1).
 */
void terminal_put_char_at(char c, uint8_t color, size_t x, size_t y);

/**
 * Writes a single character to the current cursor position and advances the cursor.
 * 
 * Handles special characters like '\n' (newline) and '\b' (backspace).
 * Performs automatic scrolling if the end of the screen is reached.
 * 
 * @param c The character to write.
 */
void terminal_write_char(char c);

/**
 * Writes a null-terminated string to the terminal.
 * 
 * @param str The string to write.
 */
void terminal_write(const char* str);

/**
 * Manually sets the hardware cursor position.
 * 
 * @param x Column.
 * @param y Row.
 */
void terminal_set_cursor_pos(size_t x, size_t y);

#ifdef __cplusplus
}
#endif

#endif
