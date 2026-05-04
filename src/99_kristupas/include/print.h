// print.h
#ifndef PRINT_H
#define PRINT_H

#include <libc/stdint.h>

void print_string(const char *s, uint8_t attrib);
void print_newline();
void scroll();
void update_cursor();
void print_backspace();

#endif