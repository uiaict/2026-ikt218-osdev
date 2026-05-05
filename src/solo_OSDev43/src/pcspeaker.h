#ifndef PCSPEAKER_H
#define PCSPEAKER_H

typedef unsigned int uint32_t;

void enable_speaker(void);
void disable_speaker(void);
void play_sound(uint32_t frequency);
void stop_sound(void);

#endif