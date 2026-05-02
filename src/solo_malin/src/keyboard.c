#include "keyboard.h"
#include "screen.h"
#include "isr.h"
#include "irq.h"
#include <libc/stdint.h>
#include <libc/stdbool.h>

#define KEYBOARD_SIZE 128
#define KEYBOARD_BUFFER_SIZE 128
#define KEYBOARD_LSHIFT 0x2A
#define KEYBOARD_RSHIFT 0x36

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint8_t buffer_head = 0;
static uint8_t buffer_tail = 0;
static bool shift_pressed = false;

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

// Shift key versions of the scancode table
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

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void keyboard_buffer_put(char c)
{
    keyboard_buffer[buffer_head] = c;
    buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

    if (buffer_head == buffer_tail) {
        buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE; 
    }
}

static void keyboard_callback(struct  InterruptRegisters* r)
{
    (void)r;

    uint8_t scancode = inb(0x60);

    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;

        if (released == KEYBOARD_LSHIFT || released == KEYBOARD_RSHIFT) {
            shift_pressed = false;
        }
        return;
    }

    if (scancode == KEYBOARD_LSHIFT || scancode == KEYBOARD_RSHIFT) {
        shift_pressed = true;
        return;
    }

    char key = 0;

    if (scancode < KEYBOARD_SIZE) {
        if (shift_pressed){
            key = keyboard_shift[scancode];
        } else {
            key = keyboard_normal[scancode];
        }
    }

    if (key != 0){
        keyboard_buffer_put(key);

        char str[2];
        str[0] = key;
        str[1] = '\0';
        write_string(str);
    }
}

void keyboard_init(void){
    irq_install_handler(1, keyboard_callback);
}

bool keyboard_has_char(void){
    return buffer_head != buffer_tail;
}

char keyboard_get_char(void){
    if (buffer_head == buffer_tail){
        return 0;
    }

    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) %KEYBOARD_BUFFER_SIZE;
    return c;
}