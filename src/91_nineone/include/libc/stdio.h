#pragma once

#include "libc/stdint.h"
#include "libc/stdbool.h"

void print_uint(uint32 n, uint8 color, int x, int y);
int putchar(int ic);
bool print(const char* data, uint64 length);
int printf(const char* __restrict__ format, ...);
int printf_color(uint8 color, const char* __restrict__ format, ...);
