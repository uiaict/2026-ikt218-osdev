#include "kernel/keyboard.h"
#include "libc/stdio.h"
#include "libc/system.h"
#include "libc/stdbool.h"

static bool shift_pressed = false;
static bool caps_lock = false;

#define BUFFER_SIZE 256
static char keyboard_buffer[BUFFER_SIZE];
static int buffer_head = 0;
static int buffer_tail = 0;

static void push_char(char c) {
    int next = (buffer_head + 1) % BUFFER_SIZE;
    if (next != buffer_tail) {
        keyboard_buffer[buffer_head] = c;
        buffer_head = next;
    }
}

char getchar() {
    while (buffer_head == buffer_tail) {
        asm volatile("hlt"); // Wait for interrupt
    }
    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
    return c;
}

bool keyboard_has_input() {
    return buffer_head != buffer_tail;
}

static const char scancode_to_ascii_lower[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0
};

static const char scancode_to_ascii_upper[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0
};

void keyboard_handler(registers_t *regs) {
    (void)regs;
    unsigned char scancode = inb(0x60);

    // Handle shift keys
    if (scancode == 0x2A || scancode == 0x36) { // Shift pressed
        shift_pressed = true;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) { // Shift released
        shift_pressed = false;
        return;
    }

    // Handle caps lock
    if (scancode == 0x3A) { // Caps lock pressed
        caps_lock = !caps_lock;
        return;
    }

    // Ignore break codes (key releases)
    if (scancode & 0x80) {
        return;
    }

    // Select the correct character based on shift and caps lock
    bool use_upper = shift_pressed ^ caps_lock;

    char c;
    if (scancode < sizeof(scancode_to_ascii_lower)) {
        char lower = scancode_to_ascii_lower[scancode];
        if (lower >= 'a' && lower <= 'z') {
            c = use_upper ? scancode_to_ascii_upper[scancode] : lower;
        } else {
            c = shift_pressed ? scancode_to_ascii_upper[scancode] : lower;
        }
    } else {
        return;
    }

    if (c == 0) {
        return;
    }

    putchar(c);
    push_char(c);
}