#pragma once
#include <stdint.h>

size_t strlen(const char* str);
int strcmp(const char* l, const char* r);

// We compile with C11 language features, so we follow the c11 std api
void* memcpy(void* restrict dest, const void* restrict src, size_t count);
void* memset(void* dest, int ch, size_t count);
