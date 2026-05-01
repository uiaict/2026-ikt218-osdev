#ifndef PIANO_H
#define PIANO_H

#include <libc/stdint.h>

void piano_init(void);
void piano_run(void);
void piano_handle_scancode(uint8_t scancode);

#endif
