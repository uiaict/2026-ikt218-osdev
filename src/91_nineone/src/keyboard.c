#include "keyboard.h"
#include "terminal.h"
#include "libc/stdbool.h"

// Simple text buffer
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint32 keyboard_buffer_index = 0;

//print position
static int keyboard_x = 1;
static int keyboard_y = 14;

// Handler used by menus or apps that want to receive keyboard events.
static keyboard_event_handler_t current_handler = 0;

// Lookup table for normal PS/2 scancodes.
static const char scancode_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' '
};

// Read the latest keyboard scancode from the PS/2 data port.
static uint8 keyboard_read_scancode(void) {
    uint8 scancode;

    __asm__ volatile ("inb %1, %0" : "=a"(scancode) : "Nd"(0x60));

    return scancode;
}

// Convert a scancode into an ASCII
char keyboard_scancode_to_ascii(uint8 scancode) {
    // Ignore scancodes outside the table
    if (scancode >= sizeof(scancode_ascii)) {
        return 0;
    }

    return scancode_ascii[scancode];
}

// Store the typed character 
static void keyboard_store_char(char c) {
    if (keyboard_buffer_index < KEYBOARD_BUFFER_SIZE - 1) {
        keyboard_buffer[keyboard_buffer_index++] = c;
        keyboard_buffer[keyboard_buffer_index] = '\0';
    }
}


 // Erase one character by moving back and writing a blank
static void keyboard_remove_last_char(void) {
    if (keyboard_buffer_index > 0) {
        keyboard_buffer_index--;
        keyboard_buffer[keyboard_buffer_index] = '\0';
    }
}


// Print keyboard input directly to the screen when no app is handling it.
static void keyboard_print_char(char c) {
    char str[2] = { c, '\0' };

    if (c == '\n') {
        keyboard_x = 0;
        keyboard_y++;
        return;
    }

    if (c == '\b') {
        if (keyboard_x > 0) {
            keyboard_x--;
            terminal_write(" ", 0x0F, keyboard_x, keyboard_y);
        }
        return;
    }

    terminal_write(str, 0x0F, keyboard_x, keyboard_y);
    keyboard_x++;

    if (keyboard_x >= VGA_WIDTH) {
        keyboard_x = 0;
        keyboard_y++;
    }

    if (keyboard_y >= VGA_HEIGHT) {
        keyboard_y = 10;
    }
}

// IRQ1 handler. runs every keyboard scancode.
static void keyboard_callback(registers_t* regs) {
    (void)regs;

    uint8 scancode = keyboard_read_scancode();

    bool is_release = (scancode & 0x80) != 0;
    char c = 0;

    // Only translate key press events to ASCII.
    // Release events are still sent to apps, but not stored as text.
    if (!is_release) {
        c = keyboard_scancode_to_ascii(scancode);
    }

    // Store typed characters in the keyboard buffer.
    if (!is_release && c != 0) {
        if (c == '\b') {
            keyboard_remove_last_char();
        } else {
            keyboard_store_char(c);
        }
    }

    // Send ALL scancodes to the current app/menu handler, including release events.
    if (current_handler != 0) {
        current_handler(c, scancode);
        return;
    }

    // If no app/menu handler exists, print normal typed characters.
    if (!is_release && c != 0) {
        keyboard_print_char(c);
    }
}

void init_keyboard(void) {
    register_interrupt_handler(IRQ1, keyboard_callback);
}

// Let an app or menu take over keyboard input temporarily.
void keyboard_set_event_handler(keyboard_event_handler_t handler) {
    current_handler = handler;
}

// Return keyboard input back to the default terminal behavior.
void keyboard_clear_event_handler(void) {
    current_handler = 0;
}

// Give other code read-only access to the typed text buffer.
const char* keyboard_get_buffer(void) {
    return keyboard_buffer;
}

void keyboard_clear_buffer(void) {
    keyboard_buffer_index = 0;
    keyboard_buffer[0] = '\0';
}

