#ifndef PCSPK_H
#define PCSPK_H

#include "libc/stdint.h"

// PC Speaker og PIT port-adresser
#define PC_SPEAKER_PORT 0x61
#define PIT_CHANNEL2_PORT 0x42
#define PIT_CMD_PORT 0x43
#define PIT_BASE_FREQ 1193180

// Port I/O helpers (må defineres et sted)
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Speaker control
void enable_speaker(void);
void disable_speaker(void);

// Play sound
void play_sound(uint32_t frequency);
void stop_sound(void);

#endif