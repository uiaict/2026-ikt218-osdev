#include "shell.h"
#include "song.h" // Sarki calabilmek icin eklendi
#include "pit.h"  // Sarkilar arasi bekleme (sleep_busy) icin eklendi
#include "matrix.h" // Matrix animasyonu icin eklendi

extern void printf(const char* format, ...);

#define CMD_BUFFER_SIZE 256
static char cmd_buffer[CMD_BUFFER_SIZE];
static char last_command[CMD_BUFFER_SIZE] = ""; // Gecmis komutu tutacak hafiza
static int cmd_index = 0;
volatile int command_ready = 0;

volatile int matrix_running = 0; // Matrixin kapanma tusu (q) icin global degisken
volatile int song_running = 0;   // Sarkiyi durdurma tusu (q) icin global degisken

void execute_command();

// Standart C kutuphanesi olmadigi icin kendi string kiyaslama fonksiyonumuzu (strcmp) yaziyoruz
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

static int command_matches_prefix(const char* command) {
    for (int i = 0; i < cmd_index; i++) {
        if (command[i] == '\0' || command[i] != cmd_buffer[i]) return 0;
    }
    return 1;
}

static void clear_current_input() {
    while (cmd_index > 0) {
        printf("\b");
        cmd_index--;
    }
}

static void write_command_to_input(const char* command) {
    clear_current_input();

    for (int i = 0; command[i] != '\0' && cmd_index < CMD_BUFFER_SIZE - 1; i++) {
        cmd_buffer[cmd_index++] = command[i];
        printf("%c", command[i]);
    }
}

void print_prompt() {
    printf("uiaos> ");
}

void shell_init() {
    printf("\n--- UiA OS Command Line Interface ---\n");
    printf("Type 'help' to see available commands.\n\n");
    print_prompt();
}

void shell_update() {
    if (command_ready) {
        execute_command();
        command_ready = 0;
    }
}

void execute_command() {
    if (cmd_index == 0) return; // Enter'a bos basildiysa gec
    cmd_buffer[cmd_index] = '\0'; // String'i sonlandir
    printf("\n");

    // Komutu isletmeden once "Gecmis (History)" hafizasina kopyaliyoruz
    for (int i = 0; i <= cmd_index; i++) {
        last_command[i] = cmd_buffer[i];
    }

    if (strcmp(cmd_buffer, "help") == 0) {
        printf("Available commands:\n");
        printf("  help   - Show this message\n");
        printf("  clear  - Clear the screen\n");
        printf("  ls     - List files in virtual RamFS\n");
        printf("  run    - Run a program (e.g. 'run matrix')\n");
        printf("  soundtest - Test PC speaker tones\n");
        printf("  tone   - Hold a PC speaker tone until 'q'\n");
    } else if (strcmp(cmd_buffer, "clear") == 0) {
        for(int i=0; i<25; i++) printf("\n"); // Basit ekran temizleme hilesi
    } else if (strcmp(cmd_buffer, "ls") == 0) {
        printf("Virtual RamFS Files:\n");
        printf("  - matrix.exe  (Executable)\n");
        printf("  - playlist.mp3(Audio Playlist)\n");
    } else if (strcmp(cmd_buffer, "run matrix") == 0 || strcmp(cmd_buffer, "run matrix.exe") == 0) {
        printf("Initializing Matrix Protocol...\n");
        printf("Press 'q' to exit.\n");
        sleep_busy(1500); // Kullanici yaziyi okuyabilsin diye ufak bir mola
        run_matrix();
        for(int i=0; i<25; i++) printf("\n"); // Matrix'ten cikinca ekrani temizle
    } else if (strcmp(cmd_buffer, "run playlist") == 0 || strcmp(cmd_buffer, "run playlist.mp3") == 0 || strcmp(cmd_buffer, "run p") == 0) {
        Song songs[] = {
            {starwars_theme, sizeof(starwars_theme) / sizeof(Note)},
            {battlefield_1942_theme, sizeof(battlefield_1942_theme) / sizeof(Note)},
            {music_1, sizeof(music_1) / sizeof(Note)}
        };
        uint32_t n_songs = sizeof(songs) / sizeof(Song);
        
        printf("Press 'q' to stop the music.\n");
        song_running = 1;
        
        for(uint32_t i = 0; i < n_songs && song_running; i++) {
            printf("Playing Song %d...\n", i + 1);
            play_song_impl(&songs[i]);
            printf("Finished playing the song.\n\n");
            
            if (i < n_songs - 1 && song_running) sleep_busy(2000); // Sarkilar arasi 2 saniye mola
        }
        song_running = 0;
    } else if (strcmp(cmd_buffer, "soundtest") == 0 || strcmp(cmd_buffer, "beep") == 0) {
        uint32_t tones[] = {C4, E4, G4, C5, G4, E4, C4};
        uint32_t n_tones = sizeof(tones) / sizeof(uint32_t);

        printf("Testing PC speaker...\n");
        for (uint32_t i = 0; i < n_tones; i++) {
            play_sound(tones[i]);
            sleep_interrupt(350);
            stop_sound();
            sleep_interrupt(80);
        }
        printf("Sound test finished.\n");
    } else if (strcmp(cmd_buffer, "tone") == 0 || strcmp(cmd_buffer, "run tone") == 0) {
        printf("Playing C4 pulse tone. Press 'q' to stop.\n");
        song_running = 1;

        while (song_running) {
            play_sound(C4);
            sleep_interrupt(120);
            stop_sound();
            sleep_interrupt(380);
        }

        stop_sound();
    } else {
        printf("Unknown command: %s\n", cmd_buffer);
    }

    cmd_index = 0; // Buffer'i sifirla
    print_prompt();
}

void shell_handle_keypress(char c) {
    if (matrix_running || song_running) {
        if (c == 'q' || c == 'Q') {
            matrix_running = 0;
            song_running = 0;
        }
        return; // Animasyon veya sarki calisiyorsa baska komut kabul etme
    }

    if (c == '\n') {
        if (!command_ready) command_ready = 1;
    } else if (c == '\b') {
        if (cmd_index > 0) {
            cmd_index--;
            printf("\b"); // kernel.c'deki putchar'imiz ekrandan harfi silecek
        }
    } else if (c == '\t') { // TAB Tusu: Otomatik Tamamlama
        if (cmd_index > 0) {
            const char* commands[] = {
                "help",
                "clear",
                "ls",
                "run matrix.exe",
                "run playlist.mp3",
                "soundtest",
                "tone"
            };
            uint32_t n_commands = sizeof(commands) / sizeof(commands[0]);

            for (uint32_t i = 0; i < n_commands; i++) {
                if (command_matches_prefix(commands[i])) {
                    write_command_to_input(commands[i]);
                    return;
                }
            }
        }
    } else if (c == 17) { // YUKARI OK: Gecmis komutu getir
        if (last_command[0] != '\0') {
            // Once mevcut yazilani sil
            while(cmd_index > 0) { printf("\b"); cmd_index--; }
            // Sonra hafizadakini yazdir
            for(int i=0; last_command[i] != '\0'; i++) {
                cmd_buffer[cmd_index++] = last_command[i];
                printf("%c", last_command[i]);
            }
        }
    } else if (c == 18) { // ASAGI OK: Yazilani komple temizle
        while(cmd_index > 0) { printf("\b"); cmd_index--; }
    } else if (c != '?') { // Tanimlanmayan karakterleri gormezden gel
        if (cmd_index < CMD_BUFFER_SIZE - 1) {
            cmd_buffer[cmd_index++] = c;
            printf("%c", c);
        }
    }
}
