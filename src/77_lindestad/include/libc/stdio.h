#pragma once

#include <libc/stdbool.h>
#include <libc/stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int putchar(int ic);
bool print(const char* data, size_t length);
int printf(const char* __restrict__ format, ...);

#ifdef __cplusplus
}
#endif
