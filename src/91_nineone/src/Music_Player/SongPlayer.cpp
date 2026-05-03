#include "song.h"
#include "libc/stdint.h"

extern "C" {
#include "../pit.h"
}

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

#define PC_SPEAKER_PORT 0x61
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL_2_PORT 0x42
#define PIT_FREQUENCY 1193182

static void enable_speaker() {
    uint8_t tmp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, tmp | 0x03);
}

static void disable_speaker() {
    uint8_t tmp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, tmp & ~0x03);
}

static void stop_sound() {
    uint8_t tmp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, tmp & ~0x03);
}

static void play_sound(uint32_t frequency) {
    if (frequency == 0) {
        stop_sound();
        return;
    }

    uint16_t divisor = (uint16_t)(PIT_FREQUENCY / frequency);

    outb(PIT_COMMAND_PORT, 0xB6);
    outb(PIT_CHANNEL_2_PORT, divisor & 0xFF);
    outb(PIT_CHANNEL_2_PORT, (divisor >> 8) & 0xFF);

    enable_speaker();
}

extern "C" void play_song_impl(Song* song) {
    for (size_t i = 0; i < song->note_count; i++) {
        Note* note = &song->notes[i];

        play_sound(note->frequency);
        sleep_interrupt(note->duration);
    }

    stop_sound();
    disable_speaker();
}