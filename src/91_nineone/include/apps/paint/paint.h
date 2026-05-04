#ifndef PAINT_H
#define PAINT_H
#include "libc/stdint.h"

void enter_paint_program();
void handle_paint_keyboard(uint8 scancode);
void tick_brush();

#endif // PAINT_H