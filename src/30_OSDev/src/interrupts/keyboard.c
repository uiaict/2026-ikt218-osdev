#include "keyboard.h"
#include "io.h"
#include "../terminal.h"

// Shift state
static int shift_pressed = 0;


volatile int key_w = 0;
volatile int key_a = 0;
volatile int key_s = 0;
volatile int key_d = 0;
volatile int key_space = 0;


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

void keyboard_isr() {
    uint8_t scancode = inb(0x60);

    if (scancode == 0x2A || scancode == 0x36)
        shift_pressed = 1;
    else if (scancode == 0xAA || scancode == 0xB6)
        shift_pressed = 0;

    // KEY PRESSED (ignore release)
    if (scancode < 128) {
        switch (scancode) {
            case 0x11: key_w = 1; break; // W
            case 0x1F: key_s = 1; break; // S
            case 0x1E: key_a = 1; break; // A
            case 0x20: key_d = 1; break; // D
            case 0x39: key_space = 1; break; // Space
        }
    }

    outb(0x20, 0x20); // EOI
}



int keyboard_read_buffer(char* out_buf, int max_len) {
    int count = (buffer_index < max_len) ? buffer_index : max_len;
    for (int i = 0; i < count; i++) {
        out_buf[i] = keyboard_buffer[i];
    }
    buffer_index = 0; // Clear buffer after read
    return count;
}