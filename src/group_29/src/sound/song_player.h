#ifndef SONG_PLAYER_H
#define SONG_PLAYER_H

#include "libc/stdint.h"

void play_default_song(void);
void play_song_by_index(uint32_t index);
uint32_t get_song_count(void);

#endif
