#ifndef TTY_H
#define TTY_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25


void terminal_write(const char* str);
void terminal_write_char(char c);
void terminal_write_hex(uint32_t value);
void terminal_write_dec(uint32_t value);
void terminal_backspace();

void terminal_put_at(char c, uint8_t color, size_t x, size_t y);
void terminal_clear();
void draw_border();

#endif