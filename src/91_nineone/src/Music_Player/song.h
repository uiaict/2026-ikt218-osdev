#pragma once

#include "libc/stdint.h"
#include "libc/stddef.h"


// One note in the song. frequency of 0 is slient
typedef struct {
    uint32_t frequency;
    uint32_t duration;
} Note;

// A song is stored as a list of notes plus the number of notes to play.
typedef struct Song {
    Note* notes;
    size_t note_count;
} Song;


// Notes for attempt at "Song of Storms" (Love Zelda). Each entry is {frequency in Hz, duration in ms}.
static Note music_1[] = {

    {294, 180},  // D4
    {349, 180},  // F4
    {587, 360},  // D5
    {0,   80},


    {294, 180},  // D4
    {349, 180},  // F4
    {587, 360},  // D5
    {0,   80},


    {659, 180},  // E5
    {698, 180},  // F5
    {659, 180},  // E5
    {698, 180},  // F5
    {659, 180},  // E5
    {523, 180},  // C5
    {440, 360},  // A4
    {0,   80},


    {440, 180},  // A4
    {587, 180},  // D5
    {698, 180},  // F5
    {784, 180},  // G5
    {880, 360},  // A5
    {0,   80},

    
    {880, 180},  // A5  
    {587, 180},  // D5
    {698, 180},  // F5
    {784, 180},  // G5
    {659, 420},  // E5

    {0,   250},
};