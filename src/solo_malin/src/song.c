#include "song.h"
#include "frequencies.h"
#include "pit.h"
#include "util.h"
#include "screen.h"
#include "memory.h"
#include "libc/stdio.h"

// ==============================
// PC speaker song player
//
// Uses PIT channel 2 + speaker
// port to play simple melodies.
// ==============================

// Enable PC speaker (connect to PIT channel 2)
void enable_speaker() {
    // Pseudocode for enable_speaker:
    // 1. Read the current state from the PC speaker control port.
    // 2. The control register bits are usually defined as follows:
    //    - Bit 0 (Speaker gate): Controls whether the speaker is on or off.
    //    - Bit 1 (Speaker data): Determines if data is being sent to the speaker.
    // 3. Set both Bit 0 and Bit 1 to enable the speaker.
    //    - Use bitwise OR operation to set these bits without altering others.

    uint8_t current_state = inPortB(PC_SPEAKER_PORT);
    // Set bits 0 and 1 to enable speaker + data
    outPortB(PC_SPEAKER_PORT, current_state | 3);
}

// Disable PC speaker
void disable_speaker() {
    // Pseudocode for disable_speaker:
    // 1. Read the current state from the PC speaker control port.
    // 2. Clear both Bit 0 and Bit 1 to disable the speaker.
    //    - Use bitwise AND with the complement of 3 (0b11) to clear these bits.
    
    uint8_t current_state = inPortB(PC_SPEAKER_PORT);
    // Clear bits 0 and 1 (speaker off)
    outPortB(PC_SPEAKER_PORT, current_state & 0xFC);
}

// Set PIT channel 2 to given frequency and start sound
void play_sound(uint32_t frequency) {
    // Pseudocode for play_sound:
    // 1. Check if the frequency is 0. If so, exit the function as this indicates no sound.
    // 2. Calculate the divisor for setting the PIT (Programmable Interval Timer) frequency.
    //    - The PIT frequency is a base value, typically 1.193182 MHz.
    //    - The divisor is PIT frequency divided by the desired sound frequency.
    // 3. Configure the PIT to the desired frequency:
    //    - Send control word to PIT control port to set binary counting, mode 3, and access mode (low/high byte).
    //    - Split the calculated divisor into low and high bytes.
    //    - Send the low byte followed by the high byte to the PIT channel 2 port.
    // 4. Enable the speaker (by setting the appropriate bits) to start sound generation.

    if(frequency == 0) return;

    // Divisor for PIT base frequency
    uint16_t divisor = PIT_BASE_FREQUENCY / frequency;

    // 0xB6: ch2, lobyte/hibyte, mode 3, binary
    outPortB(PIT_CMD_PORT, 0XB6);
    outPortB(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    outPortB(PIT_CHANNEL2_PORT, (uint8_t)((divisor >> 8) & 0xFF));

    enable_speaker();

}

// Stop currently playing sound
void stop_sound() {
    // Pseudocode for stop_sound:
    // 1. Read the current state from the PC speaker control port.
    // 2. Clear the bit that enables speaker data to stop the sound.
    //    - Use bitwise AND with the complement of the bit responsible for enabling the speaker data.

    uint8_t current_state = inPortB(PC_SPEAKER_PORT);
    // Clear bits 0 and 1 (disconnect speaker / stop tone)
    outPortB(PC_SPEAKER_PORT, current_state & ~0xFC);
}

// Core implementation: play all notes in a Song
void play_song_impl(Song *song) {
    // Pseudocode for play_song_impl:
    // 1. Enable the speaker before starting the song.
    // 2. Loop through each note in the song's notes array:
    //    a. For each note, display its details such as frequency and duration.
    //    b. Call play_sound with the note's frequency.
    //    c. Delay execution for the duration of the note (this can be implemented with a sleep function).
    //    d. Call stop_sound to end the note.
    // 3. Disable the speaker after all notes have been played.

    enable_speaker();

    for (uint32_t i = 0; i < song->length; i++){
        uint32_t frequency = song->notes[i].frequency;
        uint32_t duration = song->notes[i].duration;

        kprintf("Playing note %d: Frequency=%d, Duration=%d\n", i, song->notes[i].frequency, song->notes[i].duration);

        if(frequency == 0) {
            stop_sound();
            sleep_interrupt(duration);
        } else {
            // Play tone for the note duration
            play_sound(frequency);

            sleep_interrupt(duration);
            stop_sound();
        }
    }

    disable_speaker();
}

// Public wrapper for playing a song
void play_song(Song *song) {
    // Pseudocode for play_song:
    // 1. Call play_song_impl with the given song.
    //    - This function handles the entire process of playing each note in the song.
    
    play_song_impl(song);
}

// Create a SongPlayer object with function pointer set
SongPlayer* create_song_player(){
    SongPlayer* player = (SongPlayer*)malloc(sizeof(SongPlayer));
    player->play_song = play_song_impl;
    return player;
}

// Loop playing predefined music forever
void play_music(){
    Song songs[] = {
        {music_1, (uint32_t)(sizeof(music_1) / sizeof(Note))}
    };

    uint32_t n_songs = sizeof(songs) / sizeof(Song);
    SongPlayer* player = create_song_player();

    while (1){
        for (uint32_t i = 0; i < n_songs; i++){
            write_string("Playing Song...\n");
            player->play_song(&songs[i]);
            write_string("Finished playing the song.\n");
        }
    }
    
}
