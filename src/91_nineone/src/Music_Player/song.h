#pragma once

#include "libc/stdint.h"
#include "libc/stddef.h"

typedef struct {
    uint32_t frequency;
    uint32_t duration;
} Note;

typedef struct Song {
    Note* notes;
    size_t note_count;
} Song;

static Note music_1[] = {
    {262, 300},
    {294, 300},
    {330, 300},
    {349, 300},
    {392, 300},
    {440, 300},
    {494, 300},
    {523, 600},
    {0,   200},
};