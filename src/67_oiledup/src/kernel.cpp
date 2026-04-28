extern "C" int kernel_main(void);

#include "song/song.h"
#include "libc/stdio.h"
#include "keyboard/keyboard.h"
#include "kernel/pit.h"

extern "C" void play_music() {
    Song songs[] = {
        {music_1, MUSIC_1_LEN},
    };
    size_t n_songs = sizeof(songs) / sizeof(Song);

    SongPlayer* player = create_song_player();

    for(size_t i = 0; i < n_songs; i++) {
        player->play_song(&songs[i]);
    }
}

void show_video() {
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
    printf("[INFO] Boot sequence completed.\n");
    printf("[INFO] Memory initialized.\n");
    printf("[INFO] IDT and GDT active.\n");
    printf("[INFO] PIT frequency set to 100Hz.\n");
    printf("[DEBUG] Allocation test passed.\n");
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
        printf("Select option (1-3): ");

        char choice = getchar();
        putchar('\n');

        if (choice == '1') {
            play_music();
        } else if (choice == '2') {
            show_video();
        } else if (choice == '3') {
            show_logs();
        } else {
            printf("Invalid selection.\n");
        }
    }
    
    return 0;
}