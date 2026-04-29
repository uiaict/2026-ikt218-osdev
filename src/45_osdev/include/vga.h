#ifndef VGA_H
#define VGA_H

#include <libc/stdint.h>

// VGA color definitions
typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

typedef struct {
    uint8_t foreground;
    uint8_t background;
    uint16_t cursor_x;
    uint16_t cursor_y;
} vga_state_t;

// defining VGA dimensions
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint8_t *)0xB8000)

// Initialize VGA driver
void vga_init(void);

// Set the foreground and background colors
void vga_set_color(vga_color_t foreground, vga_color_t background);

// Put a single character at current cursor position
void vga_putchar(char c);

// Put a string on the screen
void vga_puts(const char *str);

// Clear the screen
void vga_clear(void);

// Set cursor position
void vga_set_cursor(uint16_t x, uint16_t y);

#endif

