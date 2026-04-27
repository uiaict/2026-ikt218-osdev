#include "song/song.h"
#include "speaker/speaker.h"
#include "libc/stdio.h"

static void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

void play_sound(uint32_t frequency);

void play_song_impl(Song* song) {
    enable_speaker();

    for (size_t i = 0; i < song->note_count; i++) {
        Note* note = &song->notes[i];
        if (note->frequency == 0) {
            stop_sound();
        } else {
            printf("Note: %u Hz, %u ms\n", note->frequency, note->duration);
            play_sound(note->frequency);
        }
        delay_ms(note->duration);
        stop_sound();
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