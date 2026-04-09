#include "apps/song/song.h"
#include "apps/song/song_data.h"

#include "kernel/pit.h"
#include "memory/heap.h"
#include "common.h"
#include "libc/stdio.h"

// Flag to stop music
static volatile int music_stop_flag = 0;

// Function to enable the PC speaker
void enable_speaker(){
    // Read the current state of the PC speaker control register
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    /*
    Bit 0: Speaker gate
            0: Speaker disabled
            1: Speaker enabled
    Bit 1: Speaker data
            0: Data is not passed to the speaker
            1: Data is passed to the speaker
    */
    // Check if bits 0 and 1 are not set (0 means that the speaker is disabled)
    if (speaker_state != (speaker_state | 3)) {
        // If bits 0 and 1 are not set, enable the speaker by setting bits 0 and 1 to 1
        outb(PC_SPEAKER_PORT, speaker_state | 3);
    }
}

// Function to disable the PC speaker
void disable_speaker() {
    // Turn off the PC speaker
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    // Clear bits 0 and 1 to disable the speaker
    outb(PC_SPEAKER_PORT, speaker_state & 0xFC);
}

// Function for playing a sound at a specific frequency
void play_sound(uint32_t frequency) {
    if (frequency == 0) {
        return;
    }

    uint16_t divisor = (uint16_t)(PIT_BASE_FREQUENCY / frequency);

    // Set up the PIT
    outb(PIT_CMD_PORT, 0b10110110); 
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL2_PORT, (uint8_t)(divisor >> 8));

    // Enable the speaker by setting bits 0 and 1
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    outb(PC_SPEAKER_PORT, speaker_state | 0x03);
}

// Function to stop the sound
void stop_sound(){
    // Stop the sound by disabling the gate to the speaker
    uint8_t speaker_state = inb(PC_SPEAKER_PORT);
    // Clear bits 0 and 1
    outb(PC_SPEAKER_PORT, speaker_state & ~0x03);
}

// Function for playing a song (sequence of notes)
void play_song(Song *song) {
    music_stop_flag = 0;  // Reset flag
    enable_speaker(); // Turn on the speaker

    // Loop through each note in the song
    for (uint32_t i = 0; i < song->length; i++) {
        if (music_stop_flag) {
            printf("Music stopped by user.\n");
            break; // Stop playing if the flag is set
        }
        Note* note = &song->notes[i]; // Go to the next note
        //printf("Note: %d, Freq=%d, Sleep=%d\n", i, note->frequency, note->duration); // Debug output
        play_sound(note->frequency); // Play the note
        sleep_interrupt(note->duration); // Sleep for the duration of the note
        stop_sound(); // Stop the sound
    }
    disable_speaker(); // Turn off the speaker after the song is done
}

SongPlayer* create_song_player() {
    SongPlayer* player = (SongPlayer*)malloc(sizeof(SongPlayer));
    if (player) {
        player->play_song = play_song;
    }
    return player;
}

// Function to play a specific song by index
void play_music(int song_index) {
    // Check if index is valid
    if (song_index < 0 || (uint32_t)song_index >= available_song_count) {
        printf("Invalid song index. Available songs: 0-%u\n", available_song_count - 1);
        return;
    }

    // Create a song player
    SongPlayer* player = create_song_player();
    if (!player) {
        printf("Failed to create song player.\n");
        return;
    }
    printf("Playing song %d...\n", song_index);
    
    // Play the selected song
    player->play_song(&available_songs[song_index]);
    
    printf("Finished song %d.\n", song_index);

    // Free allocated memory
    free(player);
}

void stop_music(void) {
    music_stop_flag = 1;
}
