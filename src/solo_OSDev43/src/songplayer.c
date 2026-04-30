#include "songplayer.h"
#include "pcspeaker.h"
#include "pit.h"

void terminal_write(const char* str);
void* malloc(unsigned long size);

SongPlayer* create_song_player(void) {
    SongPlayer* player = (SongPlayer*)malloc(sizeof(SongPlayer));
    player->play_song = play_song_impl;
    return player;
}

void play_song_impl(Song* song) {
    enable_speaker();

    for (uint32_t i = 0; i < song->length; i++) {
        terminal_write("Playing note\n");

        play_sound(song->notes[i].frequency);
        sleep_interrupt(song->notes[i].duration);
        stop_sound();

        sleep_interrupt(50);
    }

    disable_speaker();
}

void play_music(void) {
    Song songs[] = {
        {music_1, sizeof(music_1) / sizeof(Note)},
        {starwars_theme, sizeof(starwars_theme) / sizeof(Note)},
        {battlefield_1942_theme, sizeof(battlefield_1942_theme) / sizeof(Note)}
    };

    uint32_t n_songs = sizeof(songs) / sizeof(Song);
    SongPlayer* player = create_song_player();

    while (1) {
        for (uint32_t i = 0; i < n_songs; i++) {
            terminal_write("Playing Song...\n");
            player->play_song(&songs[i]);
            terminal_write("Finished playing the song.\n");
        }
    }
}