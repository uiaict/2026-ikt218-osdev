extern "C" {
#include "song.h"
#include <kernel/pit.h>
#include <kernel/common.h>
#include <libc/stdio.h>
#include <kernel/memory.h>

void enable_speaker(){
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    if (speaker_state != (speaker_state | 3)) {
        outb(PC_SPEAKER_PORT, speaker_state | 3);
    }
}

void disable_speaker() {
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, speaker_state & 0xFC);
}

void play_sound(uint32_t frequency) {
    if (frequency == 0) {
        return;
    }
    uint16_t divisor = (uint16_t)(PIT_BASE_FREQUENCY / frequency);
    outb(PIT_CMD_PORT, 0b10110110);
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor >> 8));
    enable_speaker();
}

void stop_sound() {
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, speaker_state & ~2);
}

void play_song_impl(Song *song) {
    enable_speaker();
    for (uint32_t i = 0; i < song->length; i++) {
        Note* note = &song->notes[i];
        play_sound(note->frequency);
        sleep_interrupt(note->duration);
        stop_sound();
    }
    disable_speaker();
}

void play_song(Song *song) {
    play_song_impl(song);
}

SongPlayer* create_song_player() {
    SongPlayer* player = (SongPlayer*)malloc(sizeof(SongPlayer));
    player->play_song = play_song_impl;
    return player;
}
}
