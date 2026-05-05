#ifndef SHELL_H
#define SHELL_H

#include "stdint.h"

extern volatile int matrix_running;
extern volatile int song_running;
extern volatile int dodger_running;

void shell_init();
void shell_update();
void shell_show_home();
void shell_handle_keypress(char c);
int shell_handle_scancode(uint8_t scancode);

#endif
