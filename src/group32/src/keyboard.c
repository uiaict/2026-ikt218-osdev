#include "keyboard.h"
#include "irq.h"
#include "ports.h"

static volatile char keyboard_buffer[256];
static volatile int keyboard_index = 0;
static volatile int extended_scancode = 0;

static const char scancode_to_ascii[128] = {
    0,   27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0,   'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0,  '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,   '*',
    0,   ' ', 0
};

static void push_key(char c) {
    if (c != 0 && keyboard_index < 255) {
        keyboard_buffer[keyboard_index] = c;
        keyboard_index++;
        keyboard_buffer[keyboard_index] = '\0';
    }
}

static void keyboard_handler(struct registers* regs) {
    (void)regs;

    unsigned char scancode = inb(0x60);

    if (scancode == 0xE0) {
        extended_scancode = 1;
        return;
    }

    if (scancode & 0x80) {
        extended_scancode = 0;
        return;
    }

    if (extended_scancode) {
        if (scancode == 0x48) {
            push_key(KEY_ARROW_UP);
        }
        else if (scancode == 0x50) {
            push_key(KEY_ARROW_DOWN);
        }

        extended_scancode = 0;
        return;
    }

    if (scancode < 128) {
        push_key(scancode_to_ascii[scancode]);
    }
}

void keyboard_install(void) {
    keyboard_index = 0;
    keyboard_buffer[0] = '\0';
    extended_scancode = 0;

    irq_install_handler(1, keyboard_handler);
}

char keyboard_get_char(void) {
    while (keyboard_index == 0) {
        __asm__ volatile ("hlt");
    }

    return keyboard_read_char();
}

char keyboard_read_char(void) {
    if (keyboard_index == 0) {
        return 0;
    }

    char c = keyboard_buffer[0];

    for (int i = 1; i < keyboard_index; i++) {
        keyboard_buffer[i - 1] = keyboard_buffer[i];
    }

    keyboard_index--;
    keyboard_buffer[keyboard_index] = '\0';

    return c;
}

int keyboard_has_key(void) {
    return keyboard_index > 0;
}