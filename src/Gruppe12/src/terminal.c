#include "terminal.h"
#include "libc/stdint.h"

// GJØR DISSE GLOBALE SÅ keyboard.c KAN BRUKE DEM
uint8_t terminal_row = 0;
uint8_t terminal_col = 0;

static volatile uint16_t* vga_buffer = (volatile uint16_t*)0xB8000;
static const uint8_t terminal_color = 0x0F;

void terminal_init() {
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = 0x00;
    }
    terminal_row = 0;
    terminal_col = 0;
}

void terminal_write(const char* str) {
    while (*str) {
        if (*str == '\n') {
            terminal_col = 0;
            terminal_row++;
        } else if (*str == '\b') {
            if (terminal_col > 0) {
                terminal_col--;
                vga_buffer[terminal_row * 80 + terminal_col] = (terminal_color << 8) | ' ';
            }
        } else {
            if (terminal_col >= 80) {
                terminal_col = 0;
                terminal_row++;
            }
            if (terminal_row >= 25) {
                terminal_row = 0;
            }
            vga_buffer[terminal_row * 80 + terminal_col] = (terminal_color << 8) | (unsigned char)*str;
            terminal_col++;
        }
        str++;
    }
}