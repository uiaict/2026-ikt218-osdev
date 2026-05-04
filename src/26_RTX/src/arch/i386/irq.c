#include <irq.h>
#include <idt.h>
#include <io.h>
#include <libc/stdio.h>
#include <libc/stdint.h>

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI   0x20

#define KB_DATA_PORT 0x60
#define KB_BUF_SIZE  256

static const char scancode_ascii[58] = {
    0,    0,   '1', '2', '3', '4', '5', '6', '7', '8',
   '9',  '0',  '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
   't',  'y',  'u', 'i', 'o', 'p', '[', ']', '\n',  0,
   'a',  's',  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'',  '`',   0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
   'm',  ',',  '.', '/',  0,  '*',  0,  ' '
};

static char     kb_buf[KB_BUF_SIZE];
static uint32_t kb_buf_head = 0;
static uint32_t kb_buf_tail = 0;

static void kb_buf_push(char c) {
    uint32_t next = (kb_buf_head + 1) % KB_BUF_SIZE;
    if (next != kb_buf_tail) {
        kb_buf[kb_buf_head] = c;
        kb_buf_head = next;
    }
}

char kb_getchar(void) {
    if (kb_buf_tail == kb_buf_head) return 0;
    char c = kb_buf[kb_buf_tail];
    kb_buf_tail = (kb_buf_tail + 1) % KB_BUF_SIZE;
    return c;
}

static irq_handler_t irq_handlers[16];

static void pic_remap(uint8_t offset1, uint8_t offset2) {
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    outb(PIC1_CMD,  0x11); io_wait();
    outb(PIC2_CMD,  0x11); io_wait();
    outb(PIC1_DATA, offset1); io_wait();
    outb(PIC2_DATA, offset2); io_wait();
    outb(PIC1_DATA, 0x04); io_wait();
    outb(PIC2_DATA, 0x02); io_wait();
    outb(PIC1_DATA, 0x01); io_wait();
    outb(PIC2_DATA, 0x01); io_wait();

    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

static void keyboard_handler(registers_t *regs) {
    (void)regs;
    uint8_t scancode = inb(KB_DATA_PORT);

    if (scancode & 0x80)
        return;

    if (scancode < sizeof(scancode_ascii)) {
        char c = scancode_ascii[scancode];
        if (c) {
            kb_buf_push(c);
            terminal_set_color(VGA_COLOR_LGREEN);
            putchar(c);
            terminal_set_color(VGA_COLOR_BWHITE);
        }
    }
}

static uint32_t timer_ticks = 0;

static void timer_handler(registers_t *regs) {
    (void)regs;
    timer_ticks++;
}

uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

void irq_install_handler(int irq, irq_handler_t handler) {
    if (irq >= 0 && irq < 16)
        irq_handlers[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    if (irq >= 0 && irq < 16)
        irq_handlers[irq] = 0;
}

void irq_init(uint16_t cs) {
    pic_remap(0x20, 0x28);

    idt_set_gate(32, (uint32_t)irq0,  cs, 0x8E);
    idt_set_gate(33, (uint32_t)irq1,  cs, 0x8E);
    idt_set_gate(34, (uint32_t)irq2,  cs, 0x8E);
    idt_set_gate(35, (uint32_t)irq3,  cs, 0x8E);
    idt_set_gate(36, (uint32_t)irq4,  cs, 0x8E);
    idt_set_gate(37, (uint32_t)irq5,  cs, 0x8E);
    idt_set_gate(38, (uint32_t)irq6,  cs, 0x8E);
    idt_set_gate(39, (uint32_t)irq7,  cs, 0x8E);
    idt_set_gate(40, (uint32_t)irq8,  cs, 0x8E);
    idt_set_gate(41, (uint32_t)irq9,  cs, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, cs, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, cs, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, cs, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, cs, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, cs, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, cs, 0x8E);

    irq_install_handler(0, timer_handler);
    irq_install_handler(1, keyboard_handler);
}

void irq_handler(registers_t *regs) {
    int irq = (int)regs->int_no - 32;

    if (irq >= 0 && irq < 16 && irq_handlers[irq])
        irq_handlers[irq](regs);

    if (irq >= 8)
        outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}
