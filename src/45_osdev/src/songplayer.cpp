#include "../include/song.h"
extern "C" {
#include "../include/pit.h"
#include "../include/libc/stdio.h"
}

//Port Addresses 
#define PC_SPEAKER_PORT    0x61   // Speaker control register
#define PIT_CHANNEL2_PORT  0x42   // PIT channel 2 data
#define PIT_COMMAND_PORT   0x43   // PIT command/mode register

// PIT Constants 
#define PIT_BASE_FREQUENCY 1193180  // Same base your pit.c uses

// Speaker Control Bits
#define SPEAKER_GATE_BIT   0x01   // Bit 0: connects PIT ch2 to speaker
#define SPEAKER_DATA_BIT   0x02   // Bit 1: enables speaker output
#define SPEAKER_ENABLE_MASK (SPEAKER_GATE_BIT | SPEAKER_DATA_BIT)

// Use the project's existing I/O functions 
extern "C" uint8_t  inb_port(uint16_t port);
extern "C" void     outb_port(uint16_t port, uint8_t val);

// Speaker Control

void enable_speaker() {
    uint8_t state = inb_port(PC_SPEAKER_PORT);
    outb_port(PC_SPEAKER_PORT, state | SPEAKER_ENABLE_MASK);
}

void disable_speaker() {
    uint8_t state = inb_port(PC_SPEAKER_PORT);
    outb_port(PC_SPEAKER_PORT, state & ~SPEAKER_ENABLE_MASK);
}


//  Sound Generation 

void play_sound(uint32_t frequency) {
    if (frequency == 0) return;  // 0 Hz = silence, nothing to do

    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;

    // Configuration for PIT channel 2:
    //   Bits [7:6] = 10  -> channel 2
    //   Bits [5:4] = 11  -> lobyte/hibyte access 
    //   Bits [3:1] = 011 -> mode 3 
    //   Bit  [0]   = 0   -> binary counting
    //   Result: 0b10110110 = 0xB6
    outb_port(PIT_COMMAND_PORT, 0xB6);

    // Send divisor to channel 2: low byte first, then high byte
    outb_port(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    outb_port(PIT_CHANNEL2_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    // Now gate the channel 2 output through to the speaker
    enable_speaker();
}

void stop_sound() {
    // Clear only the data bit to mute - gate bit stays so PIT ch2 keeps running
    uint8_t state = inb_port(PC_SPEAKER_PORT);
    outb_port(PC_SPEAKER_PORT, state & ~SPEAKER_DATA_BIT);
}


//Song Playback

extern "C" void play_song_impl(Song *song) {
    printf("Starting song (%d notes)...\n", song->note_count);
    enable_speaker();

    for (size_t i = 0; i < song->note_count; i++) {
        Note *note = &song->notes[i];

        printf("  [%d/%d] freq=%d Hz  duration=%d ms\n",
               i + 1, song->note_count,
               (unsigned)note->frequency,
               (unsigned)note->duration);

        play_sound(note->frequency);
        sleep_interrupt(note->duration);   // wait for note duration
        stop_sound();
        sleep_interrupt(10);               // 10 ms gap so notes are distinct
    }

    disable_speaker();
    printf("Song finished.\n");
}


extern "C" void play_song(Song *song) {
    play_song_impl(song);
}
