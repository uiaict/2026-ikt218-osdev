#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <libc/stdint.h>
#include <libc/stdbool.h>

// ==============================
// Keyboard input handling
//
// Provides functions to
// initialize the keyboard and
// read input characters.
// ==============================

// Initialize keyboard driver
void keyboard_init(void);

// Check if a character is available
bool keyboard_has_char(void);

// Get the next character from input
char keyboard_get_char(void);

#endif