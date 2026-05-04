#include <terminal.h>

#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/string.h>

enum {
    VGA_WIDTH = 80,
    VGA_HEIGHT = 25,
};

static const uint8_t VGA_COLOR_LIGHT_GREY = 7;
static const uint8_t VGA_COLOR_BLACK = 0;
static volatile uint16_t* const VGA_MEMORY = (uint16_t*)0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

static uint8_t vga_entry_color(uint8_t foreground, uint8_t background)
{
    return foreground | background << 4;
}

static uint16_t vga_entry(unsigned char character, uint8_t color)
{
    return (uint16_t)character | (uint16_t)color << 8;
}

static void terminal_putentryat(char character, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    VGA_MEMORY[index] = vga_entry(character, color);
}

static void terminal_scroll(void)
{
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }

    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_putentryat(' ', terminal_color, x, VGA_HEIGHT - 1);
    }

    terminal_row = VGA_HEIGHT - 1;
}

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
}

void terminal_clear(void)
{
    terminal_row = 0;
    terminal_column = 0;

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
}

void terminal_put_char_at(char character, uint8_t foreground, uint8_t background, size_t x, size_t y)
{
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return;
    }

    terminal_putentryat(character, vga_entry_color(foreground, background), x, y);
}

void terminal_putchar(char c)
{
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
        terminal_column++;
        if (terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
        }
    }

    if (terminal_row == VGA_HEIGHT) {
        terminal_scroll();
    }
}

void terminal_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data)
{
    terminal_write(data, strlen(data));
}
