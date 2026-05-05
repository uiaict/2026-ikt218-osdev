#pragma once

#include <libc/stdint.h>

/*
 * outb - write a byte to an I/O port
 * @port : 16-bit I/O port address
 * @value: byte to write
 *
 * AT&T syntax: "outb src, dst" → outb AL, DX
 * "a" constraint = AL/AX/EAX, "Nd" = immediate 0-255 or DX register.
 */
static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

/*
 * inb - read a byte from an I/O port
 * @port : 16-bit I/O port address
 * Returns the byte read.
 */
static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

/*
 * io_wait - introduce a brief I/O delay (~1 µs)
 *
 * Writing to port 0x80 (POST diagnostic port) is a harmless way to
 * delay long enough for old ISA peripherals (like the 8259 PIC) to
 * process a command byte before the next write arrives.
 */
static inline void io_wait(void)
{
    outb(0x80, 0x00);
}
