#include <libc/stddef.h>
#include <libc/stdint.h>
#include "gdt.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

static uint8_t vga_entry_color(uint8_t foreground, uint8_t background) {
    return foreground | background << 4;
}

static uint16_t vga_entry(unsigned char character, uint8_t color) {
    return (uint16_t) character | (uint16_t) color << 8;
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(15, 0);
    terminal_buffer = (uint16_t*) VGA_MEMORY;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        return;
    }

    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    terminal_buffer[index] = vga_entry(c, terminal_color);

    terminal_column++;

    if (terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }

    if (terminal_row == VGA_HEIGHT) {
        terminal_row = 0;
    }
}

void terminal_write(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i]);
    }
}

void kernel_main(void) {
    gdt_init();

    terminal_initialize();
    terminal_write("GDT Loaded!");
}