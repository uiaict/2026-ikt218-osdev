#include "arch/i386/idt.h"
#include "kernel/pit.h"
#include "stdio.h"
#include "stdint.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20
#define PIT_IRQ 32
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_IRQ 33
#define KEYBOARD_BUFFER_SIZE 256

/*
* Interrupt Service Routines (ISR)
*
* Reference:
* OSDev Wiki - Interrupt Service Routines
* https://wiki.osdev.org/Interrupt_Service_Routines
*/

extern void *isr_stub_table[];
extern void *irq_stub_table[];

// keeps the raw scancode history
static uint8_t keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint16_t keyboard_buffer_index = 0;

static void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static void io_wait(void) {
    outb(0x80, 0);
}

static const char keyboard_map[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
    'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x',
    'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

static void keyboard_logger(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // store both key press and key release scancodes
    keyboard_buffer[keyboard_buffer_index] = scancode;
    keyboard_buffer_index = (uint16_t) ((keyboard_buffer_index + 1) % KEYBOARD_BUFFER_SIZE);

    if ((scancode & 0x80) != 0) {
        return;
    }

    char key = keyboard_map[scancode];

    if (key != 0) {
        putchar(key);
    }
}

static void remap_pic(void) {
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();

    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

void init_isr(void) {
    remap_pic();

    for (uint8_t i = 0; i < 32; i++) {
        idt_set_gate(i, (uint32_t) isr_stub_table[i], 0x8E);
    }

    for (uint8_t i = 0; i < 16; i++) {
        idt_set_gate((uint8_t) (32 + i), (uint32_t) irq_stub_table[i], 0x8E);
    }
}

void isr_handler(uint32_t interrupt_number) {
    printf("Interrupt %i triggered\n", (int) interrupt_number);
}

void irq_handler(uint32_t interrupt_number) {
    // IRQ0 is the PIT and each timer interrupt advances the sleep tick counter
    if (interrupt_number == PIT_IRQ) {
        pit_tick();
    }

    if (interrupt_number == KEYBOARD_IRQ) {
        keyboard_logger();
    }

    if (interrupt_number >= 40) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}
