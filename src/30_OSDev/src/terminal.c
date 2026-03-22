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



static void terminal_scroll()
{
    uint16_t blank = (uint16_t) ' ' | (terminal_color << 8);

    if (terminal_row < VGA_HEIGHT)
        return;

    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] =
                terminal_buffer[y * VGA_WIDTH + x];
        }
    }

    // clear last line
    for (int x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }

    terminal_row = VGA_HEIGHT - 1;
}

void terminal_write_char(char c)
{
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        terminal_scroll();  
    } else {
        terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] =
            ((uint16_t)terminal_color << 8) | c;

        terminal_column++;

        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
            terminal_scroll();   
        }
    }
}

void terminal_write(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; i++) {
        terminal_write_char(str[i]);
    }
}

void terminal_backspace() {
    if (terminal_column == 0 && terminal_row == 0) return;
    if (terminal_column > 0) {
        terminal_column--;
    } else {
        terminal_row--;
        terminal_column = VGA_WIDTH - 1;
    }
    terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] = ((uint16_t)terminal_color << 8) | ' ';
}

// Convert a 32-bit value to hexadecimal and print it
void terminal_write_hex(uint32_t value) {
    char hex_digits[] = "0123456789ABCDEF";
    char buffer[9];  // 8 digits + null terminator
    buffer[8] = '\0';

    for (int i = 0; i < 8; i++) {
        // Get the lowest nibble (4 bits)
        buffer[7 - i] = hex_digits[value & 0xF];
        value >>= 4;
    }

    terminal_write(buffer); // Print the resulting hex string
}


void terminal_write_dec(uint32_t value)
{
    char buffer[11]; // maks 10 siffer + null
    int i = 10;
    buffer[i] = '\0';

    if (value == 0) {
        terminal_write("0");
        return;
    }

    while (value > 0) {
        buffer[--i] = '0' + (value % 10);
        value /= 10;
    }

    terminal_write(&buffer[i]);
}