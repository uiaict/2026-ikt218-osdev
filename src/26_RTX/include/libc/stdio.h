#pragma once

#include <libc/stdbool.h>
#include <libc/stdint.h>

int putchar(int ic);
bool print(const char* data, size_t length);
int printf(const char* __restrict__ format, ...);

void terminal_clear(void);
void terminal_set_color(unsigned char color);

/* VGA foreground color constants (background always black) */
#define VGA_COLOR_BLACK   0x00
#define VGA_COLOR_BLUE    0x01
#define VGA_COLOR_GREEN   0x02
#define VGA_COLOR_CYAN    0x03
#define VGA_COLOR_RED     0x04
#define VGA_COLOR_MAGENTA 0x05
#define VGA_COLOR_BROWN   0x06
#define VGA_COLOR_WHITE   0x07
#define VGA_COLOR_LGRAY   0x08
#define VGA_COLOR_LBLUE   0x09
#define VGA_COLOR_LGREEN  0x0A
#define VGA_COLOR_LCYAN   0x0B
#define VGA_COLOR_LRED    0x0C
#define VGA_COLOR_LMAGENTA 0x0D
#define VGA_COLOR_YELLOW  0x0E
#define VGA_COLOR_BWHITE  0x0F