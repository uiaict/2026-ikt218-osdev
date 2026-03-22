#ifndef SONG_PLAYER_H
#define SONG_PLAYER_H

#include <stdint.h>
#include "song.h"

#ifdef __cplusplus
extern "C" {
#endif

// Speaker control
void enable_speaker(void);
void disable_speaker(void);

// Sound generation
void play_sound(uint32_t frequency);
void stop_sound(void);

// Song system (const-korrekt)
void play_song_impl(const Song *song);
void play_song(const Song *song);
SongPlayer* create_song_player(void);

#ifdef __cplusplus
}
#endif

#endif