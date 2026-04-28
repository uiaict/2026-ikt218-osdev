#ifndef SONG_H
#define SONG_H

#include "frequencies.h"
#include "libc/stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

// Define a struct to represent a single musical note
typedef struct {
    uint32_t frequency; // The frequency of the note in Hz (e.g., A4 = 440 Hz)
    uint32_t duration;  // The duration of the note in milliseconds
} Note;

// Define a struct to represent a song
typedef struct {
    Note* notes;        // Pointer to an array of Note structs representing the song
    size_t note_count;    // The number of notes in the song
} Song;

// Define a struct to represent a song player
typedef struct {
    void (*play_song)(Song* song);
} SongPlayer;

// Function prototypes
SongPlayer* create_song_player();
void play_song_impl(Song* song);
void play_song(Song* song);

// Note arrays: declared as extern; definitions live in music.c
extern Note music_1[];
extern Note starwars_theme[];
extern Note battlefield_1942_theme[];
extern Note music_2[];
extern Note music_3[];
extern Note music_4[];
extern Note music_5[];
extern Note music_6[];

// Array lengths
extern const size_t music_1_len;
extern const size_t starwars_theme_len;
extern const size_t battlefield_1942_theme_len;
extern const size_t music_2_len;
extern const size_t music_3_len;
extern const size_t music_4_len;
extern const size_t music_5_len;
extern const size_t music_6_len;

// Macros for backward compatibility
#define MUSIC_1_LEN music_1_len
#define STARWARS_THEME_LEN starwars_theme_len
#define BATTLEFIELD_1942_THEME_LEN battlefield_1942_theme_len
#define MUSIC_2_LEN music_2_len
#define MUSIC_3_LEN music_3_len
#define MUSIC_4_LEN music_4_len
#define MUSIC_5_LEN music_5_len
#define MUSIC_6_LEN music_6_len

#ifdef __cplusplus
}
#endif

#endif
