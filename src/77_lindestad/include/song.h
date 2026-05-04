#pragma once

#include <libc/stdint.h>

typedef struct {
    uint32_t frequency;
    uint32_t duration;
} Note;

typedef struct {
    Note* notes;
    uint32_t length;
    const char* title;
} Song;

typedef struct {
    void (*play_song)(Song* song);
} SongPlayer;

SongPlayer* create_song_player(void);
void play_song_impl(Song* song);
void play_music(void);
