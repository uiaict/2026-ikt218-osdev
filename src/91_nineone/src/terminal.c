#include "terminal.h"
#include "colors.h"
#include "libc/stdio.h"

void terminal_putchar(uint8 c, uint8 color, int x, int y) {
    const int index = y*80 + x;
    uint16* vga_buffer = (uint16*)0xB8000;
    
    vga_buffer[index] = (uint16)c | (uint16)color << 8;
}

void terminal_setchar(uint8 c, int x, int y) {
    const int index = y * 80 + x;
    uint16* vga_buffer = (uint16*)0xB8000;
    
    // Hent nåværende verdi (tegn + farge)
    uint16 current = vga_buffer[index];
    
    // Behold fargen (øverste 8 bits), sett inn nytt tegn (nederste 8 bits)
    vga_buffer[index] = (current & 0xFF00) | (uint16)c;
}

void terminal_setcharfg(uint8 c, uint8 color_fg, int x, int y) {
    const int index = y * 80 + x;
    uint16* vga_buffer = (uint16*)0xB8000;
    
    // Hent nåværende verdi (tegn + farge)
    uint16 current = vga_buffer[index];
    
    // Behold fargen (øverste 8 bits), sett inn nytt tegn (nederste 8 bits)
    vga_buffer[index] = (current & 0xF000) | (uint16)c | ((uint16)color_fg << 8);
}

void terminal_setcolor(uint8 color, int x, int y) {
    const int index = y * 80 + x;
    uint16* vga_buffer = (uint16*)0xB8000;
    
    uint16 current = vga_buffer[index];
    
    vga_buffer[index] = (current & 0x00FF) | ((uint16)color << 8);
}

void terminal_setbgcolor(uint8 color, int x, int y) {
    const int index = y * 80 + x;
    uint16* vga_buffer = (uint16*)0xB8000;
    
    uint16 current = vga_buffer[index];
    
    vga_buffer[index] = (current & 0x0FFF) | ((uint16)color << 12);
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
        for(int x = 1; x < VGA_WIDTH-1; x++) {
            vga_buffer[y * VGA_WIDTH + x] = empty;
        }
    }

    resetRowNumber();
}

    

