#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <libc/stdint.h>
#include <libc/stdbool.h>

void keyboard_init(void);
bool keyboard_has_char(void);
char keyboard_get_char(void);

#endif