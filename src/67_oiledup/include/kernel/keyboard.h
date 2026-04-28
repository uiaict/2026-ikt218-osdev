#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "idt.h"
#include "libc/stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

void keyboard_handler(registers_t *regs);
char getchar();
bool keyboard_has_input();

#ifdef __cplusplus
}
#endif

#endif