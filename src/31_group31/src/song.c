#include "song.h"
#include "pit.h"
#include "memory.h"
#include "ports.h"
#include "shell.h"

extern void printf(const char* format, ...);

void enable_speaker() {
    // Read the current state from the PC speaker control port
    uint8_t state = port_byte_in(0x61);
    // Set Bit 0 and Bit 1 to enable the speaker
    port_byte_out(0x61, state | 3);
}

void disable_speaker() {
    // Read the current state from the PC speaker control port
    uint8_t state = port_byte_in(0x61);
    // Clear Bit 0 and Bit 1 to disable the speaker
    port_byte_out(0x61, state & 0xFC);
}

void play_sound(uint32_t frequency) {
    if (frequency == 0) return;
    
    // Calculate the PIT divisor (Base frequency: 1.193180 MHz)
    uint32_t div = 1193180 / frequency;
    
    // Send control word to PIT: Channel 2, lobyte/hibyte, Square wave, Binary
    port_byte_out(0x43, 0xB6);
    port_byte_out(0x42, (uint8_t)(div & 0xFF));
    port_byte_out(0x42, (uint8_t)(div >> 8));
    
    enable_speaker();
}

void stop_sound() {
    disable_speaker();
}

void play_song_impl(Song *song) {
    for (uint32_t i = 0; i < song->length && song_running; i++) {
        Note* note = &song->notes[i];
        if (note->frequency == 0) stop_sound();
        else play_sound(note->frequency);
        
        sleep_busy(note->duration);
        stop_sound();
        
        // Add a tiny delay to prevent notes from blending together
        sleep_busy(20);
    }
}

SongPlayer* create_song_player() {
    SongPlayer* player = (SongPlayer*)malloc(sizeof(SongPlayer));
    player->play_song = play_song_impl;
    return player;
}