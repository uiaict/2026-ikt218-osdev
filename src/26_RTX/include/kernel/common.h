#ifndef COMMON_H
#define COMMON_H

#include <io.h>
#include <libc/stdint.h>

void play_sound(uint32_t frequency);
void stop_sound(void);
void enable_speaker(void);
void disable_speaker(void);

#endif
