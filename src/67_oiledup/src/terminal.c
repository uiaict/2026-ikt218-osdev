#include "kernel/terminal.h"
#include <libc/stdint.h>
#include <libc/system.h>
#include "kernel/memory.h" // For memcpy and memset16

/**
 * VGA Text Mode Constants
 */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_BUFFER ((uint16_t*)0xB8000)

/**
 * Internal state of the terminal.
 */
static size_t terminal_row;
static size_t terminal_col;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

/**
 *  Helper to create a VGA character entry (character + attribute).
 */
static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

/**
 *  Helper to combine foreground and background colors into a single attribute byte.
 */
static inline uint8_t vga_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

/**
 *  Updates the hardware cursor position on the screen.
 * 
 * Uses I/O ports 0x3D4 and 0x3D5 to communicate with the VGA controller.
 */
static void terminal_update_hardware_cursor(void)
{
    uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

/**
 *  Scrolls the terminal content up by one line.
 * 
 * Uses memcpy to move all rows up and memset16 to clear the bottom row.
 */
static void terminal_scroll(void) {
    // Move all rows (except the first) up by one row
    // Source: row 1, Destination: row 0, Size: (HEIGHT - 1) rows
    memcpy(terminal_buffer, 
           terminal_buffer + VGA_WIDTH, 
           (VGA_HEIGHT - 1) * VGA_WIDTH * 2);

    // Clear the last row
    uint16_t blank = vga_entry(' ', terminal_color);
    memset16(terminal_buffer + (VGA_HEIGHT - 1) * VGA_WIDTH, blank, VGA_WIDTH);

    terminal_row = VGA_HEIGHT - 1;
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = vga_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = VGA_BUFFER;
    terminal_clear();
}

void terminal_clear(void) {
    uint16_t blank = vga_entry(' ', terminal_color);
    memset16(terminal_buffer, blank, VGA_WIDTH * VGA_HEIGHT);
    
    terminal_row = 0;
    terminal_col = 0;
    terminal_update_hardware_cursor();
}

void terminal_set_color(uint8_t fg, uint8_t bg) {
    terminal_color = vga_color(fg, bg);
}

void terminal_put_char_at(char c, uint8_t color, size_t x, size_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_write_char(char c) {
    // Handle newline
    if (c == '\n') {
        terminal_col = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    } 
    // Handle backspace
    else if (c == '\b') {
        if (terminal_col > 0) {
            terminal_col--;
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_col = VGA_WIDTH - 1;
        }
        // Erase character at current (new) position
        terminal_put_char_at(' ', terminal_color, terminal_col, terminal_row);
    } 
    // Handle regular character
    else {
        terminal_put_char_at(c, terminal_color, terminal_col, terminal_row);
        if (++terminal_col == VGA_WIDTH) {
            terminal_col = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_scroll();
            }
        }
    }
    
    terminal_update_hardware_cursor();
}

void terminal_write(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        terminal_write_char(str[i]);
    }
}

void terminal_set_cursor_pos(size_t x, size_t y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        terminal_col = x;
        terminal_row = y;
        terminal_update_hardware_cursor();
    }
}
