extern "C" {
    #include "song/song.h"
    #include "pit.h"
    #include "libc/stdio.h"
    #include "io.h"
    void* malloc(size_t size);
}

#define SPEAKER_PORT  0x61
#define PIT_CHANNEL2  0x42
#define PIT_CMD       0x43
#define PIT_BASE_FREQ 1193182

// Global music state - volatile because modified in interrupt context
static volatile int current_song = 0;
static volatile int current_note = 0;
static volatile uint32_t note_timer = 0;
static volatile int mute = 0;
static volatile int music_playing = 1;

void enable_speaker() {
    uint8_t state = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, state | 0x03);
}

void disable_speaker() {
    uint8_t state = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, state & ~0x03);
}

void play_sound(uint32_t frequency) {
    if (frequency == 0 || mute) {
        disable_speaker();
        return;
    }
    uint32_t divisor = PIT_BASE_FREQ / frequency;
    outb(PIT_CMD, 0xB6);
    outb(PIT_CHANNEL2, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL2, (uint8_t)((divisor >> 8) & 0xFF));
    enable_speaker();
}

void stop_sound() {
    uint8_t state = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, state & ~0x02);
}

// Called every millisecond from pit_handler via IRQ0
// Advances the song one note at a time based on note duration
extern "C" void music_tick() {
    if (!music_playing) return;

    Note* notes  = all_songs[current_song];
    uint32_t len = all_song_lengths[current_song];

    if (note_timer == 0) {
        // Time to play the next note
        play_sound(notes[current_note].frequency);
        note_timer = notes[current_note].duration;
        current_note++;

        // Loop back to start when song ends
        if (current_note >= (int)len) {
            current_note = 0;
        }
    } else {
        note_timer--;
    }
}

extern "C" void next_song() {
    current_song = (current_song + 1) % NUM_SONGS;
    current_note = 0;
    note_timer   = 0;
    stop_sound();
}

extern "C" void prev_song() {
    current_song = (current_song - 1 + NUM_SONGS) % NUM_SONGS;
    current_note = 0;
    note_timer   = 0;
    stop_sound();
}

extern "C" void toggle_mute() {
    mute = !mute;
    if (mute) stop_sound();
}

extern "C" SongPlayer* create_song_player() {
    SongPlayer* player = (SongPlayer*)malloc(sizeof(SongPlayer));
    return player;
}