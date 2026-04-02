#include "../../include/piano.h"
#include "../../include/kernel/pit.h"
#include "../../include/libc/stdio.h"
#include "song/frequencies.h"
#include "song/song.h"

static const uint32_t scancode_note_offsets[128] = {
    [0x1E] = 1,  // a
    [0x11] = 2,  // w
    [0x1F] = 3,  // s
    [0x12] = 4,  // e
    [0x20] = 5,  // d
    [0x21] = 6,  // f
    [0x22] = 7,  // g
    [0x15] = 8,  // y
    [0x23] = 9,  // h
    [0x24] = 10, // j
    [0x17] = 11, // i
    [0x25] = 12, // k
    [0x18] = 13, // o
    [0x26] = 14, // l
};

void piano_keyboard_handler(uint8_t scancode) {
  int init_offset = scancode_note_offsets[scancode];
  if (init_offset) {
    int note_offset = init_offset - 1;

    int octave = 5;
    int freq_to_play = (octave * 12) + note_offset;
    int freq = freqs[freq_to_play];
    printf("trying to play freq, %d ", freq);
    piano_play_sound(freq, 1000);
  }
}
