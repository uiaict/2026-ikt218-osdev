extern "C" int kernel_main(void);

#include "song/song.h"
#include "libc/stdio.h"

extern "C" void play_music() {
    Song songs[] = {
        {music_1, MUSIC_1_LEN},
    };
    size_t n_songs = sizeof(songs) / sizeof(Song);

    SongPlayer* player = create_song_player();

    for(size_t i = 0; i < n_songs; i++) {
        printf("Playing Song %d...\n", i + 1);
        player->play_song(&songs[i]);
        printf("Finished playing the song.\n");
    }
}

extern "C" int kernel_main(){
    
    //Allocation test
    int* alloc = new int(12);
    delete alloc;
    printf("Starting up music test...\n");
    play_music();
    
    while(1) {} 
    return 0;
}