#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "arch/i386/isr.h"

#define KEYBOARD_BUFFER_SIZE 256

// Callback type used by apps that need both ASCII characters and raw scancodes.
typedef void (*keyboard_event_handler_t)(char character, uint8 scancode);

void init_keyboard(void);
char keyboard_scancode_to_ascii(uint8 scancode);

void keyboard_set_event_handler(keyboard_event_handler_t handler);
void keyboard_clear_event_handler(void);

// Access to keyboard text buffer.
const char* keyboard_get_buffer(void);
void keyboard_clear_buffer(void);

#endif