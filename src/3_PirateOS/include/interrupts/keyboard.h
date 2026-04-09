#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "libc/stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

// Sets up the keyboard interrupt handler
void init_keyboard();

// Prints the terminal prompt
void keyboard_print_prompt(void);

// Returns the latest translated key and then clears it
char keyboard_get_last_key(void);

#ifdef __cplusplus
}
#endif

#endif
