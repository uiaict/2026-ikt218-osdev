extern "C" int kernel_main(void);

#include "song/song.h"
#include "libc/stdio.h"
#include "kernel/keyboard.h"
#include "kernel/pit.h"
#include "kernel/terminal.h"

extern "C" void play_music() {
    terminal_clear();
    printf("--- Playing Music --- \n");
    Song songs[] = {
        {music_1, MUSIC_1_LEN},
    };
    size_t n_songs = sizeof(songs) / sizeof(Song);

    SongPlayer* player = create_song_player();

    for(size_t i = 0; i < n_songs; i++) {
        player->play_song(&songs[i]);
    }
    terminal_clear();
}

void show_ascii_art() {
    terminal_clear();
    printf("--- ASCII ART 67 --- \n");
    const char* meme = 
        "   __   ______ \n"
        "  / /  |____  |\n"
        " / /_      / / \n"
        "| '_ \\    / /  \n"
        "| (_) |  / /   \n"
        " \\___/  /_/    \n"
        "               \n";
    printf("%s", meme);
}

void show_logs() {
    terminal_clear();
    printf("--- System Logs --- \n");
    printf("[INFO] Boot sequence completed.\n");
    printf("[INFO] Memory initialized.\n");
    printf("[INFO] IDT and GDT active.\n");
    printf("[INFO] PIT frequency set to 100Hz.\n");
    printf("[DEBUG] Allocation test passed.\n");
}
void show_video() {
    terminal_clear();
    printf("[VIDEO] Displaying video...\n");
    // Simulate video playback with a simple animation
    const char* frames[] = {
        "Frame 1: [=     ]\n",
        "Frame 2: [==    ]\n",
        "Frame 3: [===   ]\n",
        "Frame 4: [====  ]\n",
        "Frame 5: [===== ]\n",
        "Frame 6: [======]\n"
    };
    size_t n_frames = sizeof(frames) / sizeof(char*);

    for(size_t i = 0; i < n_frames; i++) {
        printf("%s", frames[i]);
        sleep_busy(500); // Sleep for 500ms between frames
    }
    terminal_clear();
}

extern "C" int kernel_main(){
    //Allocation test
    int* alloc = new int(12);
    delete alloc;

    while(1) {
        printf("\nGroup 67 Oiled Up\n");
        printf("1. Play Music\n");
        printf("2. Show 67 Logo\n");
        printf("3. Show Logs\n");
        printf("4. Show Video\n");
        printf("Select option (1-4): ");

        char choice = getchar();
        putchar('\n');

        if (choice == '1') {
            play_music();
        } else if (choice == '2') {
            show_ascii_art();
        } else if (choice == '3') {
            show_logs();
        } else if (choice == '4') {
            show_video();
        } else {
            printf("Invalid selection.\n");
        }
    }

    return 0;
}