#ifndef TTY_H
#define TTY_H

#include <stddef.h>
#include <stdint.h>

void terminal_write(const char* str);
void terminal_write_char(char c);
void terminal_write_hex(uint32_t value);
void terminal_write_dec(uint32_t value);
void terminal_backspace();
static void terminal_scroll();
#endif