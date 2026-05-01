extern "C" {
    #include "song/song.h"
    #include "pit.h"
    #include "libc/stdio.h"
    #include "io.h"
    void* malloc(size_t size);
}

// Port addresses for the PC speaker
// 0x61 is the keyboard controller port B, which also controls the speaker
#define SPEAKER_PORT     0x61

// PIT channel 2 is wired directly to the PC speaker
// We use it to generate the sound frequency
#define PIT_CHANNEL2     0x42

// PIT command port - we send configuration here
#define PIT_CMD          0x43

// The PIT runs at 1.193182 MHz internally
// Dividing this by our desired frequency gives us the divisor
#define PIT_BASE_FREQ    1193182


void enable_speaker() {
    // Read current state of speaker port — we must not clobber other bits
    uint8_t state = inb(SPEAKER_PORT);
    // Bit 0: speaker gate (connects PIT channel 2 to speaker)
    // Bit 1: speaker data enable
    // Set both to turn speaker on
    outb(SPEAKER_PORT, state | 0x03);
}

void disable_speaker() {
    uint8_t state = inb(SPEAKER_PORT);
    // Clear both bit 0 and bit 1 — AND with complement of 0b11
    outb(SPEAKER_PORT, state & ~0x03);
}

void play_sound(uint32_t frequency) {
    // Frequency of 0 means silence (rest note), nothing to do
    if (frequency == 0) return;

    // Calculate how many PIT ticks per wave cycle
    // Lower divisor = higher frequency, like tightening a guitar string
    uint32_t divisor = PIT_BASE_FREQ / frequency;

    // Configure PIT channel 2:
    // 0b10110110 = channel 2, lobyte/hibyte access, mode 3 (square wave), binary
    outb(PIT_CMD, 0xB6);

    // Send divisor in two bytes — low byte first, then high byte
    outb(PIT_CHANNEL2, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL2, (uint8_t)((divisor >> 8) & 0xFF));

    enable_speaker();
}

void stop_sound() {
    uint8_t state = inb(SPEAKER_PORT);
    // Clear bit 1 (speaker data) to stop the sound
    // We leave bit 0 alone — speaker stays "enabled" but silent
    outb(SPEAKER_PORT, state & ~0x02);
}

void play_song_impl(Song *song) {
    enable_speaker();

    for (uint32_t i = 0; i < song->length; i++) {
        Note note = song->notes[i];

        printf("Note %u: freq=%u Hz, duration=%u ms\n", i, note.frequency, note.duration);

        play_sound(note.frequency);
        sleep_busy(note.duration);
        stop_sound();
    }

    disable_speaker();
}

SongPlayer* create_song_player() {
    SongPlayer* player = (SongPlayer*)malloc(sizeof(SongPlayer));
    player->play_song = play_song_impl;
    return player;
}