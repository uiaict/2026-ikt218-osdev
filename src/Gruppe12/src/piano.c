#include "kernel/piano.h"
#include "kernel/pcspk.h"
#include "kernel/frequencies.h"
#include "terminal.h"
#include "libc/stdint.h"

// Piano mapping: tast -> frekvens
static const uint32_t piano_keys[8] = {
    C4, D4, E4, F4, G4, A4, B4, C5  // C4=262Hz til C5=523Hz
};

// Visuell feedback på skjermen
static void draw_piano_ui() {
    terminal_write("\n=== PIANO MODE ===\n");
    terminal_write("Trykk 1-8 for å spille:\n");
    terminal_write("1:C  2:D  3:E  4:F  5:G  6:A  7:B  8:C\n");
    terminal_write("Trykk ESC for å avslutte\n");
    terminal_write("==================\n");
}

// Håndter piano-tastetrykk
void piano_handle_key(uint8_t scancode) {
    // Ignorer key release
    if (scancode & 0x80) return;
    
    uint8_t code = scancode & 0x7F;
    
    // ESC = avslutt piano-modus
    if (code == 0x01) {
        terminal_write("\n[Exiting piano mode]\n");
        return;
    }
    
    // Tall 1-8 (scancodes 0x02-0x0B)
    if (code >= 0x02 && code <= 0x09) {
        uint8_t key_idx = code - 0x02;  // 0-7
        uint32_t freq = piano_keys[key_idx];
        
        // Visuell feedback
        char note_names[] = "CDEFGABC";
        char msg[32] = "Playing: X\n";
        msg[10] = note_names[key_idx];
        terminal_write(msg);
        
        // Spill tonen
        play_sound(freq);
        sleep_busy(200);  // 200ms note
        stop_sound();
    }
}

// Start piano-modus
void piano_init() {
    draw_piano_ui();
    terminal_write("[Piano] Ready. Press keys 1-8.\n");
}