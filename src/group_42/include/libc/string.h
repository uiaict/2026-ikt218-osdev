#pragma once
#include <stdint.h>

/**
 * Count the length of a string
 * @param str string
 * @return length of string
 */
size_t strlen(const char* str);

/**
 * Check if two strings are identical
 * @param l string
 * @param r string
 * @return 0 if identical
 */
int strcmp(const char* l, const char* r);

/**
 * Copy memory from one location to another
 * @param dest destination memory address
 * @param src source memory address
 * @param count bytes to copy
 * @return destination memory address
 */
void* memcpy(void* restrict dest, const void* restrict src, size_t count);

/**
 * Sets memory in destination memory to supplied byte
 * @param dest destination memory address
 * @param ch byte to set
 * @param count number of bytes to set
 * @return destination memory address
 */
void* memset(void* dest, int ch, size_t count);
