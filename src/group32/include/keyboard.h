#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEY_ARROW_UP 1
#define KEY_ARROW_DOWN 2

void keyboard_install(void);
char keyboard_get_char(void);
char keyboard_read_char(void);
int keyboard_has_key(void);

#endif