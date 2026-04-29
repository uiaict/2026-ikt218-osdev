#ifndef TTY_H
#define TTY_H

#include <stddef.h>
#include <stdint.h>

void terminal_write(const char* str);
void terminal_write_char(char c);

#endif