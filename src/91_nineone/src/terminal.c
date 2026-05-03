#include "terminal.h"
#include "colors.h"

void terminal_putchar(uint8 c, uint8 color, int x, int y) {
    const int index = y*80 + x;
    uint16* vga_buffer = (uint16*)0xB8000;
    
    vga_buffer[index] = (uint16)c | (uint16)color << 8;
}

void terminal_write(const char* str, uint8 color, int x, int y) {
    const uint8* u_str = (const uint8*)str;
    while (*u_str) {
        terminal_putchar(*u_str++, color, x++, y);
        if (x >= 79) { // wrapping
            x = 1;
            y++;
        }
    }
}

void terminal_clear(uint8 color) 
{
    uint16 empty = (' ' | (color << 8));
    uint16* vga_buffer = (uint16*)0xB8000;

    for(int y = 1; y < VGA_HEIGHT-1; y++) 
    {
        for(int x = 2; x < VGA_WIDTH-1; x++) {
            vga_buffer[y * VGA_WIDTH-1 + x] = empty;
        }
    }
}
