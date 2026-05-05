extern "C" int kernel_main(void);
extern "C" void print_memory_layout(void);

#include "song/song.h"
#include "libc/stdio.h"
#include "kernel/keyboard.h"
#include "kernel/video.h"
#include "kernel/memory.h"
#include "kernel/pit.h"
#include "kernel/terminal.h"

// Play the Mario theme song
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

// Ascii 67 logo lol
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

// Fake logs for demonstration purposes, could call functions, like logs(); but not in assignment.
extern "C" void show_logs() {
    terminal_clear();
    printf("Hello World!\n");
    print_memory_layout();

    void* some_memory = malloc(12345); 
    void* memory2 = malloc(54321); 
    void* memory3 = malloc(13331);

    int counter = 0;
    while(true){
        printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%d]: Slept using busy-waiting.\n", counter++);

        printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%d]: Slept using interrupts.\n", counter++);
        printf("\nPress q to quit\n");
        if (keyboard_has_input() && getchar() == 'q') {
            break;
        }
    }
}

extern "C" int kernel_main(){
    //Allocation test
    int* alloc = new int(12);
    delete alloc;

    while(1) {
        printf("\nGroup 67 Oiled Up\n");
        printf("1. Play Music\n");
        printf("2. Play Star Wars\n");
        printf("3. Show Group number\n");
        printf("4. Show tests\n");
        printf("Select option (1-4): ");

        char choice = getchar();
        putchar('\n');

        if (choice == '1') {
            play_music();
        } else if (choice == '2') {
            show_video();
        } else if (choice == '3') {
            show_ascii_art();
        } else if (choice == '4') {
            show_logs();
        } else {
            printf("Invalid selection.\n");
        }
    }

    return 0;
}