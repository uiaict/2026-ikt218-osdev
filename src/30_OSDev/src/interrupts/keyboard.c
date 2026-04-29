#include "keyboard.h"
#include "io.h"
#include "../terminal.h"

// Shift state
static int shift_pressed = 0;

// Keyboard buffer
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_index = 0;

// Scancode -> ASCII table without Shift
static char scancode_no_shift[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};

// Scancode -> ASCII table with Shift
static char scancode_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,
    'A','S','D','F','G','H','J','K','L',':','"','~',0,'|',
    'Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '
};

// ISR called by irq1 stub
void keyboard_isr() {
    uint8_t scancode = inb(0x60);  // Read from keyboard data port

    // Handle Shift press/release
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1; // Shift pressed
    } else if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0; // Shift released
    }

    // Only handle key press (ignore key release)
    if (scancode < 128) {
        char ascii = shift_pressed ? scancode_shift[scancode] : scancode_no_shift[scancode];

        if (ascii) {
            if (ascii == '\b') {  // Backspace
                if (buffer_index > 0) {
                    buffer_index--;
                    terminal_backspace();  // Remove last char on screen
                }
            } else {
                if (buffer_index < KEYBOARD_BUFFER_SIZE - 1) {
                    keyboard_buffer[buffer_index++] = ascii;  // Store in buffer
                }
                terminal_write_char(ascii); // Print to screen
            }
        }
    }

    // Send End Of Interrupt (EOI) to PIC
    outb(0x20, 0x20);
}


int keyboard_read_buffer(char* out_buf, int max_len) {
    int count = (buffer_index < max_len) ? buffer_index : max_len;
    for (int i = 0; i < count; i++) {
        out_buf[i] = keyboard_buffer[i];
    }
    buffer_index = 0; // Clear buffer after read
    return count;
}