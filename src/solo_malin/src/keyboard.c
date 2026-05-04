#include "keyboard.h"
#include "screen.h"
#include "isr.h"
#include "irq.h"
#include <libc/stdint.h>
#include <libc/stdbool.h>

// ==============================
// PS/2 keyboard driver
//
// Handles IRQ1, translates scan
// codes to ASCII, and buffers
// characters for reading.
// ==============================

#define KEYBOARD_SIZE 128
#define KEYBOARD_BUFFER_SIZE 128
#define KEYBOARD_LSHIFT 0x2A
#define KEYBOARD_RSHIFT 0x36

// Ring buffer for keyboard characters
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint8_t buffer_head = 0;
static uint8_t buffer_tail = 0;
static bool shift_pressed = false;

// Scan code to ASCII table (no modifier)
const unsigned char keyboard_normal[KEYBOARD_SIZE] = {
    0,    '\x1B', '1',  '2',  '3',  '4',  '5',  '6',
    '7',  '8',    '9',  '0',  '+',  '\'', '\b', '\t',
    'q',  'w',    'e',  'r',  't',  'y',  'u',  'i',
    'o',  'p',    'a',  'u',  '\r', 0,    'a',  's',
    'd',  'f',    'g',  'h',  'j',  'k',  'l',  'o',
    'a',  '^',    0,    '<',  'z',  'x',  'c',  'v',
    'b',  'n',    'm',  ',',  '.',  '-',  0,    0,
    0,    ' ',    0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0
};

// Scan code to ASCII table (with Shift pressed)
const unsigned char keyboard_shift[KEYBOARD_SIZE] = {
    0,    '\x1B', '!',  '"',  '#',  '$',  '%',  '&',
    '/',  '(',    ')',  '=',  '?',  '`',  '\b', '\t',
    'Q',  'W',    'E',  'R',  'T',  'Y',  'U',  'I',
    'O',  'P',    'A',  'U',  '\r', 0,    'A',  'S',
    'D',  'F',    'G',  'H',  'J',  'K',  'L',  'O',
    'A',  '*',    0,    '>',  'Z',  'X',  'C',  'V',
    'B',  'N',    'M',  ';',  ':',  '_',  0,    0,
    0,    ' ',    0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0,
    0,    0,      0,    0,    0,    0,    0,    0
};

// Read a byte from an I/O port (local helper)
static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

// Push a character into the ring buffer (overwrite oldest if full)
static void keyboard_buffer_put(char c)
{
    keyboard_buffer[buffer_head] = c;
    buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

    // If buffer is full, drop oldest character
    if (buffer_head == buffer_tail) {
        buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE; 
    }
}

// IRQ1 callback: handle keyboard interrupt
static void keyboard_callback(struct  InterruptRegisters* r)
{
    (void)r;    // Unused parameter

    uint8_t scancode = inb(0x60);        // Read scan code from keyboard controller

     // Key release (high bit set)
    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;

         // Track Shift key release
        if (released == KEYBOARD_LSHIFT || released == KEYBOARD_RSHIFT) {
            shift_pressed = false;
        }
        return;
    }

    // Key press for Shift keys
    if (scancode == KEYBOARD_LSHIFT || scancode == KEYBOARD_RSHIFT) {
        shift_pressed = true;
        return;
    }

    char key = 0;

    // Translate scan code to ASCII
    if (scancode < KEYBOARD_SIZE) {
        if (shift_pressed){
            key = keyboard_shift[scancode];
        } else {
            key = keyboard_normal[scancode];
        }
    }

    // Store printable key in buffer
    if (key != 0){
        keyboard_buffer_put(key);
        
        // Simple debug marker for keypress
        write_string("*");
        /*
        char str[2];
        str[0] = key;
        str[1] = '\0';
        write_string(str);
        */
    }
}

// Initialize keyboard driver (attach to IRQ1)
void keyboard_init(void){
    irq_install_handler(1, keyboard_callback);
}

// Check if there is a character available
bool keyboard_has_char(void){
    return buffer_head != buffer_tail;
}

// Get next character from buffer (0 if none)
char keyboard_get_char(void){
    if (buffer_head == buffer_tail){
        return 0;
    }

    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) %KEYBOARD_BUFFER_SIZE;
    return c;
}