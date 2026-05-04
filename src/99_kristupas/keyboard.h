#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <libc/stdint.h>

void keyboard_init();
void keyboard_handler();
uint8_t get_last_key();

#endif