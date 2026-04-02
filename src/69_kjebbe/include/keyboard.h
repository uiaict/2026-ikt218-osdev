#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../include/libc/stdint.h"
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_BUFFER_SIZE 128

typedef struct {
  int buffer[KEYBOARD_BUFFER_SIZE];
  int front;
  int back;
} KeyboardBuffer;

void kb_enqueue(KeyboardBuffer *kb, int entry);
int kb_dequeue(KeyboardBuffer *kb);
void keyboard_init(void);

extern KeyboardBuffer kb;
extern const char scancode_ascii[128];

int get_key(int scancode);
void keyboard_handler_common(int scancode);
void keyboard_handler(int scancode);

#endif
