#ifndef SONG_PLAYER_H
#define SONG_PLAYER_H

#include "libc/stdint.h"

// Plays the first built-in song during boot.
void play_default_song(void);

// Plays one of the built-in songs by index.
void play_song_by_index(uint32_t index);

// Returns the number of built-in songs available for playback.
uint32_t get_song_count(void);

#endif
