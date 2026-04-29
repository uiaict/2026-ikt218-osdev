#include "terminal.h"
#include "colors.h"

void terminal_putchar(char c, uint8 color, int x, int y) {
    const int index = y*80 + x;
    uint16* vga_buffer = (uint16*)0xB8000;
    
    vga_buffer[index] = (uint16)c | (uint16)color << 8;
}

void terminal_write(const char* str, uint8 color, int x, int y) {
    while (*str) {
        terminal_putchar(*str++, color, x++, y);
        if (x >= 80) { // wrapping
            x = 0;
            y++;
        }
    }
}