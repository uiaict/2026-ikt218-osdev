#include "keyboard.h"
#include "ports.h"
#include "isr.h"
#include "shell.h"

// We need our custom printf to display the characters
extern void printf(const char* format, ...);

// Task 4: Lookup table to translate keyboard scancodes into ASCII characters.
// This is a basic US QWERTY layout table.
const char sc_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '?', 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', '\n', '?', 'a', 's', 'd', 'f', 'g',
    'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '
};

static void keyboard_callback(registers_t *regs) {
    // The PIC sends the keyboard scancode to port 0x60.
    // We use our port_byte_in function to read it.
    uint8_t scancode = port_byte_in(0x60);

    if (shell_handle_scancode(scancode)) {
        return;
    }
    
    if (scancode == 0x48) {
        shell_handle_keypress(17);
        return;
    } else if (scancode == 0x50) {
        shell_handle_keypress(18);
        return;
    } else if (scancode == 0x0F) {
        shell_handle_keypress('\t');
        return;
    }

    // We only care about key presses for now.
    // Key releases usually generate scancodes higher than 128.
    // Our lookup table only covers up to index 57.
    if (scancode > 57) {
        return; 
    }

    // Translate the scancode to an ASCII character
    char letter = sc_ascii[scancode];
    
    shell_handle_keypress(letter);
}

void init_keyboard() {
    // Register our callback function for IRQ1 (which is the PS/2 Keyboard)
    register_interrupt_handler(IRQ1, keyboard_callback);
}
