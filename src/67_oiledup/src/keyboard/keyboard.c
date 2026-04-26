#include "keyboard.h"
#include "../../include/libc/stdio.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static const char scancode_to_ascii[] = {
    0,   0,  '1', '2', '3', '4', '5', '6',
   '7', '8', '9', '0', '-', '=',  0,   0,
   'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
   'o', 'p', '[', ']',  0,   0,  'a', 's',
   'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
   '\'','`',  0,  '\\','z', 'x', 'c', 'v',
   'b', 'n', 'm', ',', '.', '/',  0,   0,
    0,  ' '
};

void keyboard_handler(void) {
    unsigned char scancode;

    scancode = inb(0x60);
    
    if (scancode & 0x80) {
        return;
    }

    char c = scancode_to_ascii[scancode];

    if (c == 0) {
        return;
    }
    
    putchar(c);
}