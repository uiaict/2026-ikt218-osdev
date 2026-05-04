#include <interrupts.h>

#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/stdio.h>
#include <ports.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_pointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

enum {
    IDT_ENTRY_COUNT = 256,
    KERNEL_CODE_SELECTOR = 0x08,
    IDT_INTERRUPT_GATE = 0x8E,
    PIC1_COMMAND = 0x20,
    PIC1_DATA = 0x21,
    PIC2_COMMAND = 0xA0,
    PIC2_DATA = 0xA1,
    PIC_EOI = 0x20,
};

static struct idt_entry idt_entries[IDT_ENTRY_COUNT];
static struct idt_pointer idt_ptr;
static interrupt_handler_t interrupt_handlers[IDT_ENTRY_COUNT];
static char keyboard_buffer[256];
static uint32_t keyboard_buffer_length;

extern void idt_flush(uint32_t idt_ptr_address);
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

static const char keyboard_scancode_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0,  'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0,  '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0,  '*',
    0,  ' ', 0,
};

static void idt_set_entry(uint8_t index, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt_entries[index].base_low = (uint16_t)(base & 0xFFFF);
    idt_entries[index].selector = selector;
    idt_entries[index].zero = 0;
    idt_entries[index].flags = flags;
    idt_entries[index].base_high = (uint16_t)((base >> 16) & 0xFFFF);
}

static void pic_remap(void)
{
    uint8_t master_mask = port_byte_in(PIC1_DATA);
    uint8_t slave_mask = port_byte_in(PIC2_DATA);

    port_byte_out(PIC1_COMMAND, 0x11);
    port_io_wait();
    port_byte_out(PIC2_COMMAND, 0x11);
    port_io_wait();

    port_byte_out(PIC1_DATA, 0x20);
    port_io_wait();
    port_byte_out(PIC2_DATA, 0x28);
    port_io_wait();

    port_byte_out(PIC1_DATA, 0x04);
    port_io_wait();
    port_byte_out(PIC2_DATA, 0x02);
    port_io_wait();

    port_byte_out(PIC1_DATA, 0x01);
    port_io_wait();
    port_byte_out(PIC2_DATA, 0x01);
    port_io_wait();

    (void)master_mask;
    (void)slave_mask;

    port_byte_out(PIC1_DATA, 0xFC);
    port_byte_out(PIC2_DATA, 0xFF);
}

static void pic_send_eoi(uint8_t interrupt_number)
{
    if (interrupt_number >= 40) {
        port_byte_out(PIC2_COMMAND, PIC_EOI);
    }

    if (interrupt_number >= 32) {
        port_byte_out(PIC1_COMMAND, PIC_EOI);
    }
}

static void keyboard_handler(struct interrupt_registers* registers)
{
    (void)registers;

    uint8_t scancode = port_byte_in(0x60);
    if ((scancode & 0x80) != 0 || scancode >= sizeof(keyboard_scancode_ascii)) {
        return;
    }

    char ascii = keyboard_scancode_ascii[scancode];
    if (ascii == 0) {
        return;
    }

    if (keyboard_buffer_length < sizeof(keyboard_buffer)) {
        keyboard_buffer[keyboard_buffer_length++] = ascii;
    }

    printf("%c", ascii);
}

void interrupt_register_handler(uint8_t interrupt_number, interrupt_handler_t handler)
{
    interrupt_handlers[interrupt_number] = handler;
}

void interrupts_initialize(void)
{
    idt_ptr.limit = (uint16_t)(sizeof(idt_entries) - 1);
    idt_ptr.base = (uint32_t)&idt_entries;

    for (uint32_t i = 0; i < IDT_ENTRY_COUNT; i++) {
        idt_set_entry((uint8_t)i, 0, KERNEL_CODE_SELECTOR, 0);
        interrupt_handlers[i] = NULL;
    }

    idt_set_entry(0, (uint32_t)isr0, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(1, (uint32_t)isr1, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(2, (uint32_t)isr2, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);

    pic_remap();

    idt_set_entry(32, (uint32_t)irq0, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(33, (uint32_t)irq1, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(34, (uint32_t)irq2, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(35, (uint32_t)irq3, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(36, (uint32_t)irq4, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(37, (uint32_t)irq5, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(38, (uint32_t)irq6, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(39, (uint32_t)irq7, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(40, (uint32_t)irq8, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(41, (uint32_t)irq9, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(42, (uint32_t)irq10, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(43, (uint32_t)irq11, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(44, (uint32_t)irq12, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(45, (uint32_t)irq13, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(46, (uint32_t)irq14, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);
    idt_set_entry(47, (uint32_t)irq15, KERNEL_CODE_SELECTOR, IDT_INTERRUPT_GATE);

    interrupt_register_handler(33, keyboard_handler);
    idt_flush((uint32_t)&idt_ptr);
}

void interrupt_handler(struct interrupt_registers* registers)
{
    interrupt_handler_t handler = interrupt_handlers[registers->interrupt_number];
    if (handler != NULL) {
        handler(registers);
    } else if (registers->interrupt_number < 32) {
        printf("software interrupt %u triggered\n", registers->interrupt_number);
    }

    pic_send_eoi((uint8_t)registers->interrupt_number);
}
