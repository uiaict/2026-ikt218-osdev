#include <piano/piano.h>
#include <kernel/pit.h>
#include <kernel/common.h>
#include <libc/stdio.h>
#include <io.h>

#define NUM_KEYS 8

static const uint8_t piano_scancodes[NUM_KEYS] = {
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25
};

static const uint32_t piano_freqs[NUM_KEYS] = {
    262, 294, 330, 349, 392, 440, 494, 523
};

static const char piano_keys[NUM_KEYS] = {
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K'
};

static const char *piano_notes[NUM_KEYS] = {
    "C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5"
};

static int active_key = -1;

static void draw_piano(int active) {
    printf("\n  +----+----+----+----+----+----+----+----+\n");
    printf("  |");
    for (int i = 0; i < NUM_KEYS; i++) {
        if (i == active) {
            terminal_set_color(VGA_COLOR_LGREEN);
            printf(" %c  |", piano_keys[i]);
            terminal_set_color(VGA_COLOR_BWHITE);
        } else {
            printf(" %c  |", piano_keys[i]);
        }
    }
    printf("\n  +----+----+----+----+----+----+----+----+\n");
    printf("  |");
    for (int i = 0; i < NUM_KEYS; i++) {
        if (i == active) {
            terminal_set_color(VGA_COLOR_LGREEN);
            printf(" %s |", piano_notes[i]);
            terminal_set_color(VGA_COLOR_BWHITE);
        } else {
            printf(" %s |", piano_notes[i]);
        }
    }
    printf("\n  +----+----+----+----+----+----+----+----+\n");
    if (active >= 0) {
        printf("\n  >> Playing: %s (%d Hz)\n", piano_notes[active], piano_freqs[active]);
    } else {
        printf("\n  >> Press A S D F G H J K to play notes\n");
    }
}

void piano_init(void) {
    terminal_clear();
    terminal_set_color(VGA_COLOR_LCYAN);
    printf("=== UiAOS Piano ===\n\n");
    terminal_set_color(VGA_COLOR_BWHITE);
    printf("  Keys: A=C4  S=D4  D=E4  F=F4  G=G4  H=A4  J=B4  K=C5\n\n");
    draw_piano(-1);
}

void piano_handle_scancode(uint8_t scancode) {
    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        for (int i = 0; i < NUM_KEYS; i++) {
            if (piano_scancodes[i] == released && active_key == i) {
                stop_sound();
                active_key = -1;
                draw_piano(-1);
                return;
            }
        }
        return;
    }
    for (int i = 0; i < NUM_KEYS; i++) {
        if (piano_scancodes[i] == scancode) {
            if (active_key != i) {
                active_key = i;
                play_sound(piano_freqs[i]);
                draw_piano(i);
            }
            return;
        }
    }
}
