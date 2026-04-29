#pragma once

#include "song/song.h"

SongPlayer* create_song_player();
void play_song_impl(Song* song);
void play_song(Song* song);