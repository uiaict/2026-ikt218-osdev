#include "speaker/speaker.h"

#define PIT_FREQ 1193182

#define SPEAKER_PORT 0x61
#define PIT_CTRL_PORT 0x43
#define PIT_CH2_PORT 0x42

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void delay(uint32_t ticks) {
    for (volatile uint32_t i = 0; i < ticks; i++);
}

void enable_speaker() {
    uint8_t val = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, val | 0x03);
}

void disable_speaker() {
    uint8_t val = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, val & ~0x03);
}

void play_sound(uint32_t frequency) {
    if (frequency == 0) return;

    uint32_t divisor = PIT_FREQ / frequency;

    outb(PIT_CTRL_PORT, 0xB6);
    outb(PIT_CH2_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CH2_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    enable_speaker();
}

void stop_sound() {
    uint8_t val = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, val & ~0x02);
}