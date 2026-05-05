#ifndef DODGER_H
#define DODGER_H

#include "stdint.h"

void run_dodger();
void dodger_handle_key(char c);
void dodger_handle_scancode(uint8_t scancode);

#endif
