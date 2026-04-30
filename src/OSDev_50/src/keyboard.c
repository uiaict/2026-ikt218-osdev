#include <keyboard.h>
#include <interrupts.h>
#include <common.h>
#include <input.h>
#include <terminal.h>

#define KBD_BUF_SIZE 128

static uint8_t buf[KBD_BUF_SIZE];
static uint32_t head = 0;

static void push_scancode(uint8_t sc)
{
    buf[head % KBD_BUF_SIZE] = sc;
    head++;
}

static void keyboard_irq1(registers_t* regs, void* ctx)
{
    (void)regs; (void)ctx;

    uint8_t sc = inb(0x60);
    push_scancode(sc);

    char c = scancode_to_ascii(sc);
    if (c) {
        char s[2];
        s[0] = c;
        s[1] = 0;
        terminal_write(s);
    }
}

void keyboard_init(void)
{
    register_irq_handler(1, keyboard_irq1, 0);
}
