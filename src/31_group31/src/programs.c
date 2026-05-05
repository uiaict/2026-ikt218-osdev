#include "programs.h"
#include "dodger.h"
#include "matrix.h"
#include "pit.h"
#include "shell.h"
#include "song.h"
#include "vfs.h"

extern void printf(const char* format, ...);

static void run_matrix_program() {
    printf("Launching Matrix Rain demo\n");
    printf("Controls: ESC exit, c color, f/s speed, p pause, r reset.\n");
    matrix_running = 1;
    sleep_busy(1500);
    if (!matrix_running) {
        shell_show_home();
        return;
    }
    run_matrix();
    shell_show_home();
}

static void run_dodger_program() {
    printf("Launching Signal Dodger\n");
    printf("Controls: a/d move, p pause, m music, ESC exit.\n");
    dodger_running = 1;
    sleep_busy(1500);
    if (!dodger_running) {
        shell_show_home();
        return;
    }
    run_dodger();
    shell_show_home();
}

static void run_playlist_program() {
    Song songs[] = {
        {starwars_theme, sizeof(starwars_theme) / sizeof(Note)},
        {battlefield_1942_theme, sizeof(battlefield_1942_theme) / sizeof(Note)},
        {music_1, sizeof(music_1) / sizeof(Note)}
    };
    uint32_t n_songs = sizeof(songs) / sizeof(Song);

    printf("Launching PC speaker playlist\n");
    printf("Press ESC to stop the music.\n");
    song_running = 1;

    for (uint32_t i = 0; i < n_songs && song_running; i++) {
        printf("Playing song %d...\n", i + 1);
        play_song_impl(&songs[i]);
        printf("Finished song %d.\n\n", i + 1);

        if (i < n_songs - 1 && song_running) sleep_busy(2000);
    }
    song_running = 0;
    shell_show_home();
}

static void run_soundtest_program() {
    uint32_t tones[] = {C4, E4, G4, C5, G4, E4, C4};
    uint32_t n_tones = sizeof(tones) / sizeof(uint32_t);

    printf("PC speaker diagnostic melody\n");
    for (uint32_t i = 0; i < n_tones; i++) {
        play_sound(tones[i]);
        sleep_interrupt(350);
        stop_sound();
        sleep_interrupt(80);
    }
    printf("Diagnostic finished.\n");
}

static void run_tone_program() {
    printf("Tone generator active. Press ESC to stop.\n");
    song_running = 1;

    while (song_running) {
        play_sound(C4);
        sleep_interrupt(120);
        stop_sound();
        sleep_interrupt(380);
    }

    stop_sound();
    shell_show_home();
}

void program_run_app(int app) {
    if (app == VFS_APP_MATRIX) run_matrix_program();
    else if (app == VFS_APP_DODGER) run_dodger_program();
    else if (app == VFS_APP_PLAYLIST) run_playlist_program();
    else if (app == VFS_APP_SOUNDTEST) run_soundtest_program();
    else if (app == VFS_APP_TONE) run_tone_program();
}
