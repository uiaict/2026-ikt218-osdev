#include "kernel/song.h"
#include "kernel/pcspk.h"
#include "kernel/pit.h"
#include "terminal.h"

// Spill én note
static void play_note(Note* note) {
    if (note->frequency == 0) {
        stop_sound();
        sleep_busy(note->duration);
    } else {
        play_sound(note->frequency);
        sleep_busy(note->duration);
        stop_sound();
    }
}

// Spill hele sangen
void play_song_impl(Song *song) {
    if (!song || !song->notes) return;
    
    enable_speaker();
    
    for (uint32_t i = 0; i < song->length; i++) {
        play_note(&song->notes[i]);
        sleep_busy(50);  // Kort pause mellom noter
    }
    
    disable_speaker();
}

void play_song(Song *song) {
    play_song_impl(song);
}

// SongPlayer factory (C-versjon)
SongPlayer* create_song_player() {
    // Enkel implementasjon - returnerer en statisk player
    static SongPlayer player;
    player.play_song = play_song_impl;
    return &player;
}