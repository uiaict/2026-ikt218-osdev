#include "keyboard.h"
#include "ports.h"
#include "isr.h"

// We need our custom printf to display the characters
extern void printf(const char* format, ...);

// Task 4: Lookup table to translate keyboard scancodes into ASCII characters.
// This is a basic US QWERTY layout table.
const char sc_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
    'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '
};

static void keyboard_callback(registers_t *regs) {
    // The PIC sends the keyboard scancode to port 0x60.
    // We use our port_byte_in function to read it.
    uint8_t scancode = port_byte_in(0x60);
    
    // We only care about key presses for now.
    // Key releases usually generate scancodes higher than 128.
    // Our lookup table only covers up to index 57.
    if (scancode > 57) {
        return; 
    }

    // Translate the scancode to an ASCII character
    char letter = sc_ascii[scancode];
    
    // Print the character directly to the screen!
    printf("%c", letter);
}

void init_keyboard() {
    // Register our callback function for IRQ1 (which is the PS/2 Keyboard)
    register_interrupt_handler(IRQ1, keyboard_callback);
}