#pragma once
#include <../include/libc/stdint.h>
#include <../include/libc/stddef.h>

// Note frequencies in Hz 
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_REST  0   // silence

// Data structures for frequenzy and duration

typedef struct {
    uint32_t frequency;   // Hz  (0 = rest/silence)
    uint32_t duration;    // milliseconds
} Note;

typedef struct {
    Note*  notes;
    size_t note_count;
} Song;

// simple Test song: Twinkle Twinkle (first phrase) 
// Each note is 400 ms; rests between repeated notes prevent them blurring.
static Note music_1[] = {
    {NOTE_C4, 400},   // Twin-
    {NOTE_C4, 400},   // -kle
    {NOTE_G4, 400},   // twin-
    {NOTE_G4, 400},   // -kle
    {NOTE_A4, 400},   // lit-
    {NOTE_A4, 400},   // -tle
    {NOTE_G4, 600},   // star
    {NOTE_REST,200},
    {NOTE_F4, 400},   // how
    {NOTE_F4, 400},   // I
    {NOTE_E4, 400},   // won-
    {NOTE_E4, 400},   // -der
    {NOTE_D4, 400},   // what
    {NOTE_D4, 400},   // you
    {NOTE_C4, 600},   // are
    {NOTE_REST,200},
}; 
