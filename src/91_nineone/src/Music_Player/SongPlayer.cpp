#include "song.h"
#include "libc/stdint.h"

extern "C" {
#include "../pit.h"
}

// write one byte to I/O port
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// read byte from I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

#define PC_SPEAKER_PORT 0x61
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL_2_PORT 0x42
#define PIT_FREQUENCY 1193182

// Enable speaker. Bit 0 connects PIT channel 2 to speaker. Bit 1 enables the speaker output.
static void enable_speaker() {
    uint8_t tmp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, tmp | 0x03);
}

// Disable speaker. Clears bit 0 and 1.
static void disable_speaker() {
    uint8_t tmp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, tmp & ~0x03);
}

// stop current tone.
static void stop_sound() {
    uint8_t tmp = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, tmp & ~0x03);
}

// PIT channel 2 generates tone at given frequency.
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

// Notes program PIT channel 2 with a frequency, then waits for note duration with PIT.
extern "C" void play_song_impl(Song* song) {
    if (song == 0 || song->notes == 0 || song->note_count == 0) {
        return;
    }

    stop_sound();
    enable_speaker();

    for (size_t i = 0; i < song->note_count; i++) {
        Note* note = &song->notes[i];

        play_sound(note->frequency);
        sleep_interrupt(note->duration);

        stop_sound(); // Prevent smearing of notes.
        sleep_interrupt(10);
    }
  
    stop_sound();
    disable_speaker();

    
}