#include "song_player.h"
#include "song/song.h"
#include "pit.h"
#include "ports.h"
#include "memory.h"
#include "libc/stdint.h"

#define PIT_COMMAND_PORT   0x43
#define PIT_CHANNEL2_PORT  0x42
#define PC_SPEAKER_PORT    0x61

static void enable_speaker() {
    uint8_t value = inb(PC_SPEAKER_PORT);
    if ((value & 0x03) != 0x03) {
        outb(PC_SPEAKER_PORT, value | 0x03);
    }
}

static void disable_speaker() {
    uint8_t value = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, value & 0xFC);
}

static void play_sound(uint32_t frequency) {
    if (frequency == 0) {
        return;
    }

    uint32_t divisor = PIT_BASE_FREQUENCY / frequency;

    outb(PIT_COMMAND_PORT, 0xB6);
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL2_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    enable_speaker();
}

static void stop_sound() {
    uint8_t value = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, value & 0xFC);
}


void play_song_impl(Song* song) {
    if (!song || !song->notes || song->length == 0) {
        return;
    }

    enable_speaker();

    for (uint32_t i = 0; i < song->length; i++) {
        Note note = song->notes[i];

        if (note.frequency == 0) {
            stop_sound();
        } else {
            play_sound(note.frequency);
        }

        pit_sleep_ms(note.duration);
        stop_sound();
    }

    disable_speaker();
}

void play_song(Song* song) {
    play_song_impl(song);
}

SongPlayer* create_song_player() {
    SongPlayer* player = (SongPlayer*)malloc(sizeof(SongPlayer));
    if (!player) {
        return 0;
    }

    player->play_song = play_song;
    return player;
}