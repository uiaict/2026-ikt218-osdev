#pragma once
#include <stdbool.h>
#include <stdint.h>


// register the keyboard interrupt handler
void init_keyboard();

// set keyboard to use scancode set 2
void keyboard_set_scancode_set2();

// decode scancodes into the keybuffer
void decode_keyboard();


// pop a key off the keybuffer
bool pop_key(uint8_t* out);


bool has_special_key();
// pop a key off the special keybuffer (arrows, home end, etc)
bool pop_special_key(uint8_t* out);
