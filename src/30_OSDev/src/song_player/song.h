#ifndef SONG_H
#define SONG_H

#include "frequencies.h"
#include <stdint.h>


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
   const Note* notes;        // Pointer to an array of Note structs representing the song
    uint32_t length;    // The number of notes in the song
} Song;

// Define a struct to represent a song player
typedef struct {
    void (*play_song)(const Song* song); // Function pointer to a function that plays a song
} SongPlayer;

// Function prototype for creating a new SongPlayer instance
// Returns a pointer to a newly created SongPlayer object



void play_song_impl(const Song *song);
void play_song(const Song *song);
void play_music(void);

// Musikk – KUN deklarasjoner her:
extern const Note music_1[];            extern const uint32_t music_1_len;
extern const Note starwars_theme[];     extern const uint32_t starwars_theme_len;
extern const Note battlefield_1942_theme[]; extern const uint32_t battlefield_1942_theme_len;
extern const Note music_2[];            extern const uint32_t music_2_len;
extern const Note music_3[];            extern const uint32_t music_3_len;
extern const Note music_4[];            extern const uint32_t music_4_len;
extern const Note music_5[];            extern const uint32_t music_5_len;
extern const Note music_6[];            extern const uint32_t music_6_len;

#ifdef __cplusplus
}
#endif

#endif // SONG_H
