#include "../include/pic.h"
#include <../include/libc/stdint.h>

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2+1)

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void pic_remap(int offset1, int offset2) {

    outb(PIC1_COMMAND, 0x11); // init
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();
    outb(PIC1_DATA, offset1); // vector offset
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();
    outb(PIC1_DATA, 4); // tell Master about slave at IRQ2
    io_wait();
    outb(PIC2_DATA, 2); // tell Slave its cascade identity
    io_wait();
    outb(PIC1_DATA, 0x01); // 8086 mode
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

   // Unmask only IRQ0 (PIT) and IRQ1 (Keyboard), we mask everything else
   outb(PIC1_DATA, 0b11111100);
   outb(PIC2_DATA, 0xFF);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8)
        outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);
}
uint8_t inb_port(uint16_t port) { return inb(port); }
void outb_port(uint16_t port, uint8_t val) { outb(port, val); }

