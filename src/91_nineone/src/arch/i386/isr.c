#include "arch/i386/isr.h"
#include "arch/i386/idt.h"

#include "terminal.h"

static int isr_line = 0;

static isr_t interrupt_handlers[256];

static inline void outb(uint16 port, uint8 value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void print_uint(uint32 n, uint8 color, int x, int y) {
    char buffer[11];
    int i = 0;

    if (n == 0) {
        terminal_write("0", color, x, y);
        return;
    }

    while (n > 0) {
        buffer[i++] = '0' + (n % 10);
        n /= 10;
    }

    char out[12];
    int j = 0;

    while (i > 0) {
        out[j++] = buffer[--i];
    }

    out[j] = '\0';

    terminal_write(out, color, x, y);
}


void register_interrupt_handler(uint8 n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void isr_handler(registers_t* regs) {
    terminal_write("Interrupt triggered: ", 0x0F, 0, isr_line);
    terminal_write("   ", 0x0F, 21, isr_line);

    print_uint(regs->int_no, 0x0F, 21, isr_line);

    isr_line++;

    if (isr_line >= VGA_HEIGHT) {
        isr_line = 0;
    }
}


void irq_handler(registers_t* regs) {
    /*
     * Call registered IRQ handler if one exists.
     * Example: IRQ1 keyboard handler.
     */
    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }

    /*
     * Send EOI to PICs.
     * IRQ8-IRQ15 come from the slave PIC, so they need EOI to both.
     */
    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);
}





static inline uint8 inb(uint16 port) {
    uint8 result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
#define KEYBOARD_BUFFER_SIZE 256

static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint32 keyboard_buffer_index = 0;

static int keyboard_x = 1;
static int keyboard_y = 14;

static char scancode_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' '
};

static void keyboard_store_char(char c) {
    if (keyboard_buffer_index < KEYBOARD_BUFFER_SIZE - 1) {
        keyboard_buffer[keyboard_buffer_index++] = c;
        keyboard_buffer[keyboard_buffer_index] = '\0';
    }
}

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

void keyboard_callback(registers_t* regs) {
    (void)regs;

    uint8 scancode = inb(0x60);

    /*
     * Ignore key release events.
     * PS/2 release scancodes usually have bit 7 set.
     */
    if (scancode & 0x80) {
        return;
    }

    if (scancode < sizeof(scancode_ascii)) {
        char c = scancode_ascii[scancode];

        if (c != 0) {
            keyboard_store_char(c);
            keyboard_print_char(c);
        }
    }
}