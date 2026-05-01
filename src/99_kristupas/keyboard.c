#include "keyboard.h"
#include "include/print.h"
#include "include/io.h"

extern void next_song();
extern void prev_song();
extern void toggle_mute();

// Scancode to ASCII lookup table
// Index is the scancode, value is the ASCII character
// 0 means no printable character for that scancode
static char scancode_table[128] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=', 0,
    0, 'q','w','e','r','t','y','u','i','o','p','[',']', 0,
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' '
};
static int shift_pressed = 0;
static int ctrl_pressed = 0;
static volatile uint8_t last_key = 0;


void keyboard_handler() {
    // Read the scancode from the keyboard data port
    uint8_t scancode = inb(0x60);

    // Track shift key state
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_pressed = 0;
        return;
    }

     // Track Ctrl key state (left ctrl = 0x1D)
    if (scancode == 0x1D) { ctrl_pressed = 1; return; }
    if (scancode == 0x9D) { ctrl_pressed = 0; return; }

        // Ctrl combos
    if (ctrl_pressed) {
        if (scancode == 0x32) { toggle_mute(); return; } // Ctrl+M
        if (scancode == 0x31) { next_song(); return; }   // Ctrl+N  
        if (scancode == 0x19) { prev_song(); return; }   // Ctrl+P
        return;
    }
    
    // Only handle key press events, not key release (release has bit 7 set)
    if (scancode & 0x80) {
        return;
    }

    //enter
    if (scancode == 0x1C) { 
        last_key = '\n';
        print_newline();
        return;
    }

    if (scancode == 0x0E) { 
        last_key = '\b';
        print_backspace();
        return;
    }

    // Look up the ASCII character and print it if printable
    char c = scancode_table[scancode];
    if (c != 0) {
        // If shift is held, convert to uppercase
        if (shift_pressed && c >= 'a' && c <= 'z') {
            c -= 32;
        }
        last_key = c;
        char str[2] = {c, 0};
        print_string(str, 0x07);
    }
}

void keyboard_init() {
    // Register keyboard_handler for IRQ1 (PS/2 keyboard)
    // Now irq_handler will call it automatically when IRQ1 fires
    irq_install_handler(1, keyboard_handler);
}

uint8_t get_last_key() {
    uint8_t k = last_key;
    last_key = 0;
    return k;
}