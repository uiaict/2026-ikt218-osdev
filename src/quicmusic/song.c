#include "song.h"
#include "libc/stdio.h"
#include "kernel_memory.h"
#include "libc/stddef.h"

//convers an array of notes into a song object
Song* notes_to_song(Note* notes_arr[], uint32_t notes_arr_size) {
    uint32_t song_length = 0;
    
    for (uint32_t note_index = 0; note_index < notes_arr_size; note_index++) {
        song_length += notes_arr[note_index]->duration;
    }

    Song* song = malloc(sizeof(Song));
    if (!song) return NULL;
    song->length = song_length;
    song->notes = malloc(notes_arr_size * sizeof(Note*));
    // Memcpy isn't implemented.
    for (uint32_t i = 0; i < notes_arr_size; ++i) {
        song->notes[i] = *notes_arr[i];
    }
    return song;
};

//Allocates a songplayer in the heap
SongPlayer* create_song_player() {
    SongPlayer* player = malloc(sizeof(SongPlayer));

    return player;
};

//Plays a song
void* play_song(Song* song) {
    while (1) {

    }
}

//Plays all songs
void play_music() {
    // How to play music
    Song songs[] = {
        {music_1, sizeof(music_1) / sizeof(Note)}
    };
    uint32_t n_songs = sizeof(songs) / sizeof(Song);

    SongPlayer* player = create_song_player();

    while(1) {
        for(uint32_t i = 0; i < n_songs; i++) {
            printf("Playing Song...\n");
            player->play_song(&songs[i]);
            printf("Finished playing the song.\n");
        }
    }
}