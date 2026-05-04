#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <libc/stdint.h>

void keyboard_install();

char keyboard_getchar(void);

char keyboard_check(void);


#endif

