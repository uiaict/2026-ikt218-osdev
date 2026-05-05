#include "irq.h"

extern char keymap[128];
void terminal_write(const char* str);
void terminal_putchar(char c);
void idt_set_gate(unsigned char num, unsigned int base, unsigned short selector, unsigned char flags);
void pit_tick(void);

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

static inline void outb(unsigned short port, unsigned char value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void play_sound(unsigned int frequency);
void stop_sound(void);
void piano_status(const char* note);

void irq_handler(unsigned int irq_number) {
    if (irq_number == 0) {
        pit_tick();
        
    } else if (irq_number == 1) {
        unsigned char scancode = inb(0x60);

        // Ignores key releases
        if (!(scancode & 0x80)) {
            char c = keymap[scancode];

            
            if (c == '1') {
                play_sound(262);
                piano_status("C4");
            } else if (c == '2') {
                play_sound(294);
                piano_status("D4");
            } else if (c == '3') {
                play_sound(330);
                piano_status("E4");
            } else if (c == '4') {
                play_sound(349);
                piano_status("F4");
            } else if (c == '5') {
                play_sound(392);
                piano_status("G4");
            } else if (c == '6') {
                play_sound(440);
                piano_status("A4");
            } else if (c == '7') {
                play_sound(494);
                piano_status("B4");
            } else if (c == '8') {
                play_sound(523);
                piano_status("C5");
            } else if (c == ' ') {
                stop_sound();
                piano_status("Stopped");
            }
        }
    }

    // Send End Of Interrupt to PIC
    if (irq_number >= 8) {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);
}

static void pic_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20); // IRQ0-7 mapped to interrupt 32-39
    outb(0xA1, 0x28); // IRQ8-15 mapped to interrupt 40-47

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void irq_install(void) {
    pic_remap();

    idt_set_gate(32, (unsigned int)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)irq1, 0x08, 0x8E);
    idt_set_gate(34, (unsigned int)irq2, 0x08, 0x8E);
    idt_set_gate(35, (unsigned int)irq3, 0x08, 0x8E);
    idt_set_gate(36, (unsigned int)irq4, 0x08, 0x8E);
    idt_set_gate(37, (unsigned int)irq5, 0x08, 0x8E);
    idt_set_gate(38, (unsigned int)irq6, 0x08, 0x8E);
    idt_set_gate(39, (unsigned int)irq7, 0x08, 0x8E);
    idt_set_gate(40, (unsigned int)irq8, 0x08, 0x8E);
    idt_set_gate(41, (unsigned int)irq9, 0x08, 0x8E);
    idt_set_gate(42, (unsigned int)irq10, 0x08, 0x8E);
    idt_set_gate(43, (unsigned int)irq11, 0x08, 0x8E);
    idt_set_gate(44, (unsigned int)irq12, 0x08, 0x8E);
    idt_set_gate(45, (unsigned int)irq13, 0x08, 0x8E);
    idt_set_gate(46, (unsigned int)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned int)irq15, 0x08, 0x8E);

    asm volatile("sti");
}