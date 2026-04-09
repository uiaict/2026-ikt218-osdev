#include "interrupts/keyboard.h"
#include "interrupts/isr.h"
#include "kernel/cli.h"
#include "common.h"
#include "apps/raycaster/raycaster.h"
#include "libc/stdio.h"

// Scancode to ASCII lookup table (US QWERTY layout)
const char kbdUS[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',     /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,      /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`',   0,        /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
  'm', ',', '.', '/',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

const char kbdUS_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
  '(', ')', '_', '+', '\b', /* Backspace */
  '\t',     /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,      /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
 '"', '~',   0,        /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
  'M', '<', '>', '?',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

#define KBD_BUFFER_SIZE 256
char keyboard_buffer[KBD_BUFFER_SIZE];
int buffer_index = 0;
int shift_pressed = 0;
static int extended_scancode = 0;
static volatile char last_key_pressed = 0;

static void handle_extended_scancode(uint8_t scancode)
{
    uint8_t released = scancode & 0x80;
    uint8_t code = scancode & 0x7F;

    if (released) {
        return;
    }

    if (raycaster_input_is_active()) {
        return;
    }

    if (code == 0x49) {
        terminal_scroll_page_up();
        return;
    }

    if (code == 0x51) {
        terminal_scroll_page_down();
        return;
    }

    if (code == 0x48) {
        terminal_scroll_line_up();
        return;
    }

    if (code == 0x50) {
        terminal_scroll_line_down();
        return;
    }

    if (code == 0x47) {
        terminal_scroll_to_top();
        return;
    }

    if (code == 0x4F) {
        terminal_scroll_to_bottom();
    }
}

static void keyboard_callback(registers_t *regs) {
    (void)regs;
    // The PIC leaves us the scancode in port 0x60
    uint8_t scancode = inb(0x60);

    if (raycaster_input_is_active()) {
        raycaster_input_submit_scancode(scancode);
    }

    if (scancode == 0xE0) {
        extended_scancode = 1;
        return;
    }

    if (extended_scancode) {
        extended_scancode = 0;
        handle_extended_scancode(scancode);
        return;
    }

    // Top bit set means key released
    if (scancode & 0x80) {
        // Key release
        uint8_t released_key = scancode & ~0x80;
        if (released_key == 0x2A || released_key == 0x36) { // Left shift (42) or Right shift (54)
            shift_pressed = 0;
        }

    } else {
        // Key press
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
        } else {
            char ascii = shift_pressed ? kbdUS_shift[scancode] : kbdUS[scancode];
            
            // ESC key - always handle specially
            if (scancode == 0x01) {
                ascii = 27;  // ESC code
                last_key_pressed = ascii;
                raycaster_input_request_exit();
                if (!raycaster_input_is_active()) {
                    cli_handle_escape();
                }
                return;
            }
            
            // Always capture the key for games
            last_key_pressed = ascii;

            // In game mode, only capture keys - don't process commands
            if (raycaster_input_is_active()) {
                return;
            }
            
            // Normal command mode - process as commands
            if (ascii == '\b') {
                terminal_scroll_to_bottom();
                if (buffer_index > 0) {
                    buffer_index--;
                    keyboard_buffer[buffer_index] = '\0';
                    printf("\b \b");
                }
            } else if (ascii == '\n') {
                terminal_scroll_to_bottom();
                cli_submit_line(keyboard_buffer);
                buffer_index = 0;
                keyboard_buffer[0] = '\0';
            } else if (ascii != 0) {
                terminal_scroll_to_bottom();
                // Store in buffer
                if (buffer_index < KBD_BUFFER_SIZE - 1) {
                    keyboard_buffer[buffer_index++] = ascii;
                    keyboard_buffer[buffer_index] = '\0';
                }
                // Print out character
                printf("%c", ascii);
            }
        }
    }
}

void init_keyboard() {
    keyboard_buffer[0] = '\0';
    register_interrupt_handler(IRQ1, keyboard_callback);
}

void keyboard_print_prompt(void)
{
    cli_print_prompt();
}

char keyboard_get_last_key(void)
{
    char key = last_key_pressed;
    last_key_pressed = 0;  // Clear after reading
    return key;
}
