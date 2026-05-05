#ifndef PIANO_H
#define PIANO_H

#include "libc/stdint.h"

// Initialiser piano-modus
void piano_init(void);

// Håndter tastetrykk i piano-modus
void piano_handle_key(uint8_t scancode);

#endif