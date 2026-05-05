#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "libc/stdint.h"

void keyboard_init(void);
void keyboard_handler(uint32_t scancode);
char scancode_to_ascii(uint8_t scancode);

#endif