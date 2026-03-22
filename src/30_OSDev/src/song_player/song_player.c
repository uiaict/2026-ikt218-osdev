#include <stdint.h>
#include "song.h"
#include "libc/stdio.h"
#include "io.h"
#include "../pit/pit.h"



void enable_speaker() {
    uint8_t tmp = inb(0x61);

   
 // sett bit 0 og 1
    if (tmp != (tmp | 3)) {
        outb(0x61, (uint8_t)(tmp | 3));
    }
}


void disable_speaker() {
    uint8_t tmp = inb(0x61);
    tmp &= 0xFC; // fjern bit 0 og 1
    outb(0x61, tmp);
}

void play_sound(uint32_t frequency) {

    // 1. ingen lyd hvis pause
    if (frequency == 0) return;

    // 2. regn ut timing
    uint32_t divisor = 1193182 / frequency;

    // 3. konfigurer PIT til square wave
    outb(0x43, 0xB6);

    // 4. send frekvens (low + high byte)
    outb(0x42, (uint8_t)(divisor & 0xFF));
    outb(0x42, (uint8_t)(divisor >> 8));

    // 5. koble speaker på
    enable_speaker();
}

void stop_sound() {
    uint8_t tmp = inb(0x61); //Les port 0x61
    tmp &= 0xFD; // fjern bit 1
    outb(0x61, tmp);
}

void play_song_impl(const Song *song) {
    
    if (!song || !song->notes || song->length == 0) return;


    // 1. koble PIT på speaker før vi spiller noe
    enable_speaker();

    // 2. spill alle noter
    for (uint32_t i = 0; i < song->length; i++) {

        Note note = song->notes[i]; //hent note

        // spill tone
        play_sound(note.frequency); 

        // hold tonen(duration: hvor lenge note varer)
        sleep_interrupt(note.duration); 

        // stopp tone
        stop_sound();
    }

    // 3. koble av speaker
    disable_speaker();
}

void play_song(const Song *song) {
    play_song_impl(song);
}

 SongPlayer* create_song_player(void)
{
    static SongPlayer player;
    player.play_song = play_song;
    return &player;
}


void play_music(void) {
    Song song = {
        .notes  = music_1,
        .length = music_1_len
    };
    play_song(&song);
}
