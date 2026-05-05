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
// Non-blocking playback using PIT ticks (for star wars)
void start_song(Song* song);
void stop_song(void);

// Note arrays: declared as extern; definitions live in music.c
extern Note music_1[];
extern Note starwars_theme[];

// Array lengths
extern const size_t music_1_len;
extern const size_t starwars_theme_len;

// Macros for backward compatibility
#define MUSIC_1_LEN music_1_len
#define STARWARS_THEME_LEN starwars_theme_len

#ifdef __cplusplus
}
#endif

#endif
