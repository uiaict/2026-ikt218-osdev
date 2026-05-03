#ifndef SCREEN_H
#define SCREEN_H

#include <libc/stdint.h>
#include <libc/stdarg.h>

void scroll(void);
void write_string(const char *s);
void write_char(char c);
void write_dec(uint32_t n);
void write_hex(uint32_t n);
void kprintf(const char* fmt, ...);

#endif