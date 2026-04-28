#include "song.h"
#include "speaker/speaker.h"
#include "libc/stdio.h"
#include "kernel/pit.h"

void play_song_impl(Song* song) {
    enable_speaker();

    for (size_t i = 0; i < song->note_count; i++) {
        Note* note = &song->notes[i];
        printf("Playing note %u\n", (unsigned int)(i + 1));
        if (note->frequency == 0) {
            stop_sound();
        } else {
            play_sound(note->frequency);
        }
        sleep_interrupt(note->duration);
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