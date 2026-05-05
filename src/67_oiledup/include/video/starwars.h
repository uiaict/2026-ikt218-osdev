#ifndef STARWARS_H
#define STARWARS_H

#include "libc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* starwars_frames[];
extern const int starwars_durations[];
extern const size_t starwars_frame_count;

#ifdef __cplusplus
}
#endif

#endif // STARWARS_H
