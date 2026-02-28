#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint8_t etc.
#include "terminal.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VIDEO_MEMORY ((uint16_t*)0xB8000)

static uint16_t* terminal_buffer = VIDEO_MEMORY;
static uint8_t terminal_row = 0;
static uint8_t terminal_column = 0;
static uint8_t terminal_color = 0x0F; 

void terminal_write_char(char c)
{
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] = ((uint16_t)terminal_color << 8) | c;
        terminal_column++;
        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
        }
    }
}

void terminal_write(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i++) {
        terminal_write_char(str[i]);
    }
}