#include "menu.h"
#include "screen.h"
#include "keyboard.h"
#include "song.h"
#include "pong.h"
#include "pit.h"

// ==============================
// Main menu
//
// Displays the main menu and
// handles user input for starting
// programs or halting the system.
// ==============================


// Menu text shown on screen
static const char* main_menu =
    "----- Main Menu -----\n"
    "1. Music player\n"
    "2. Pong game\n"
    "Q. Halt\n";

// Clear any pending keyboard input
static void flush_keyboard_buffer(void) {
    while (keyboard_has_char()) {
        keyboard_get_char();
    }
}

// Clear any pending keyboard input
static void print_main_menu(void) {
    clearTerminal();              // Clear screen
    write_string(main_menu);      // Print menu text
    write_string("Choose option:\n");
}

// Run the main menu loop
void run_main_menu(void) {
    while (1) {
        print_main_menu();
        flush_keyboard_buffer();

        // Wait for a valid key press
        while (1) {
            if (keyboard_has_char()) {
                char c = keyboard_get_char();

                if (c == '1') {
                    play_music();         // Run music player
                    break;                // Return to menu after finish
                } else if (c == '2') {
                    run_pong();           // Run Pong game
                    break;                // Return to menu after finish
                } else if (c == 'q' || c == 'Q') {
                    write_string("Halting...\n");

                    // Stop the CPU forever
                    for (;;) {
                        __asm__ __volatile__("cli; hlt");
                    }
                }
            }

            // Small delay to avoid tight busy polling
            sleep_interrupt(20);
        }
    }
}