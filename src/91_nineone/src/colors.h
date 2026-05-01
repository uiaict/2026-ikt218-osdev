#ifndef COLORS_H
#define COLORS_H

#include "libc/stdint.h" // For å få tak i uint8

// Vga colors (4b fg, 4b bg)
#define BLACK         0
#define BLUE          1
#define GREEN         2
#define CYAN          3
#define RED           4
#define MAGENTA       5
#define BROWN         6
#define LIGHT_GREY    7
#define DARK_GREY     8
#define LIGHT_BLUE    9
#define LIGHT_GREEN   10
#define LIGHT_CYAN    11
#define LIGHT_RED     12
#define LIGHT_MAGENTA 13
#define YELLOW        14
#define WHITE         15

// Pack foreground and background into a single byte
#define COLOR(f, b) ((uint8)((f) | ((b) << 4)))

#endif