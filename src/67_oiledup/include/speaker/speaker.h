#pragma once

#include "libc/stdint.h"

void enable_speaker();
void disable_speaker();
void play_sound(uint32_t frequency);
void stop_sound();