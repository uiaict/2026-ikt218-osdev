#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../idt/idt.h"

#ifdef __cplusplus
extern "C" {
#endif

void keyboard_handler(registers_t *regs);
char getchar();

#ifdef __cplusplus
}
#endif

#endif