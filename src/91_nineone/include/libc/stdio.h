#pragma once

#include "libc/stdint.h"
#include "libc/stdbool.h"

void print_uint(uint32_t n, uint8_t color, int x, int y);
int putchar(int ic);
bool print(const char* data, uint64 length);
int printf(const char* __restrict__ format, ...);
