#pragma once

#include "libc/stdbool.h"
#include "libc/stdint.h"

#define KEYBOARD_BUFFER_SIZE 256

typedef struct {
    uint8_t buffer[KEYBOARD_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} scancode_buffer_t;

typedef struct {
    uint16_t keycode;
    bool release;
} keycode_t;

typedef struct {
    keycode_t buffer[KEYBOARD_BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} keycode_buffer_t;

/// @brief Pushes a raw keyboard scancode into the scancode buffer.
/// 
/// @param buffer The scancode ring buffer to write to.
/// @param scancode The raw byte read from the keyboard controller.
void push_scancode_buffer(scancode_buffer_t* buffer, uint8_t scancode);

/// @brief Pops the oldest raw scancode from the scancode buffer.
/// 
/// @param buffer The scancode ring buffer to read from.
/// @return The oldest scancode in the buffer, or 0 if the buffer is empty.
uint8_t pop_scancode_buffer(scancode_buffer_t* buffer);

/// @brief Creates and returns an empty scancode ring buffer.
/// 
/// @return A zero-initialized scancode buffer.
scancode_buffer_t create_scancode_buffer(void);

/// @brief Pushes a decoded key event into the keycode buffer.
/// 
/// @param buffer The keycode ring buffer to write to.
/// @param keycode The decoded key event to store.
void push_keycode_buffer(keycode_buffer_t* buffer, keycode_t keycode);

/// @brief Pops the oldest key event from the keycode buffer.
/// 
/// @param buffer The keycode ring buffer to read from.
/// @return The oldest key event in the buffer, or a zero-initialized keycode if empty.
keycode_t pop_keycode_buffer(keycode_buffer_t* buffer);

/// @brief Creates and returns an empty keycode ring buffer.
/// 
/// @return A zero-initialized keycode buffer.
keycode_buffer_t create_keycode_buffer(void);

/// @brief Converts one buffered scancode into a keycode event.
/// 
/// @param input The input scancode buffer.
/// @param output The output keycode buffer.
void scancode2keycode(scancode_buffer_t* input, keycode_buffer_t* output);

/// @brief Creates a keycode struct describing one key press or release event.
/// 
/// @param keycode The decoded keyboard key identifier.
/// @param release True if the key was released, false if it was pressed.
/// @return A populated keycode struct.
keycode_t new_keycode(uint16_t keycode, bool release);

/// @brief Initializes keyboard buffers and modifier state.
void init_keyboard(void);

/// @brief Processes one raw scancode from the keyboard controller.
/// 
/// @param scancode The raw byte read from port `0x60`.
void keyboard_handle_scancode(uint8_t scancode);

/// @brief Checks whether a translated ASCII character is available.
/// 
/// @return True if the ASCII buffer contains at least one character.
bool keyboard_has_char(void);

/// @brief Pops the oldest translated ASCII character.
/// 
/// @return The oldest character in the ASCII buffer, or 0 if the buffer is empty.
char keyboard_pop_char(void);
