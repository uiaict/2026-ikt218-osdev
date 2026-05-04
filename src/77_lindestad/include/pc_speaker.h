#pragma once

#include <libc/stdint.h>

#define PIT_CHANNEL2_PORT 0x42
#define PC_SPEAKER_PORT 0x61

void enable_speaker(void);
void disable_speaker(void);
void play_sound(uint32_t frequency);
void stop_sound(void);
