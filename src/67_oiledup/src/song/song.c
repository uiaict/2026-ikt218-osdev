#include "song/song.h"
#include "speaker/speaker.h"
#include "libc/stdio.h"
#include "kernel/pit.h"

void play_song_impl(Song* song) {
    enable_speaker();

    for (size_t i = 0; i < song->note_count; i++) {
        Note* note = &song->notes[i];
        if (note->frequency == 0) {
            stop_sound();
        } else {
            play_sound(note->frequency);
        }
        sleep_interrupt(note->duration);
        stop_sound();
        /* short rest between notes to reduce bleed and perceived loudness */
        sleep_busy(10);
    }

    disable_speaker();
}

SongPlayer* create_song_player() {
    static SongPlayer player = {play_song_impl};
    return &player;
}

void play_song(Song* song) {
    play_song_impl(song);
}

/* Non-blocking song player using PIT ticks */
static Song* current_song = 0;
static size_t current_index = 0;
static uint32_t remaining_ms = 0;
static int playing = 0;

static void song_tick(void) {
    if (!playing || !current_song) return;

    if (remaining_ms > 0) {
        remaining_ms--;
        return;
    }

    if (current_index >= current_song->note_count) {
        /* finished */
        stop_sound();
        stop_song();
        return;
    }

    Note* note = &current_song->notes[current_index++];
    if (note->frequency == 0) {
        stop_sound();
    } else {
        play_sound(note->frequency);
    }
    remaining_ms = note->duration;
}

void start_song(Song* song) {
    if (!song) return;
    current_song = song;
    current_index = 0;
    remaining_ms = 0;
    playing = 1;
    register_pit_tick_callback(song_tick);
}

void stop_song(void) {
    playing = 0;
    current_song = 0;
    register_pit_tick_callback(0);
    stop_sound();
}