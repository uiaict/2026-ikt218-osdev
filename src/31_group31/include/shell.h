#ifndef SHELL_H
#define SHELL_H

extern volatile int matrix_running;
extern volatile int song_running;

void shell_init();
void shell_update();
void shell_handle_keypress(char c);

#endif