#include "pcspeaker.h"

#define PIT_BASE_FREQUENCY 1193180
#define PIT_CMD_PORT 0x43
#define PIT_CHANNEL2_PORT 0x42
#define PC_SPEAKER_PORT 0x61

static inline void outb(unsigned short port, unsigned char value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void enable_speaker(void) {
    unsigned char tmp = inb(PC_SPEAKER_PORT);
    if ((tmp & 3) != 3) {
        outb(PC_SPEAKER_PORT, tmp | 3);
    }
}

void disable_speaker(void) {
    unsigned char tmp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, tmp & 0xFC);
}

void play_sound(uint32_t frequency) {
    if (frequency == 0) {
        return;
    }

    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;

    outb(PIT_CMD_PORT, 0xB6);
    outb(PIT_CHANNEL2_PORT, divisor & 0xFF);
    outb(PIT_CHANNEL2_PORT, (divisor >> 8) & 0xFF);

    enable_speaker();
}

void stop_sound(void) {
    unsigned char tmp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, tmp & 0xFC);
}