#include <song.h>

#include <brahms_op38_opening.h>
#include <libc/stdio.h>
#include <pc_speaker.h>
#include <pit.h>

static SongPlayer player = {
    .play_song = play_song_impl,
};

SongPlayer* create_song_player(void)
{
    return &player;
}

void play_song_impl(Song* song)
{
    if (song == 0 || song->notes == 0 || song->length == 0) {
        return;
    }

    printf("starting song: %s\n", song->title);
    enable_speaker();

    for (uint32_t i = 0; i < song->length; i++) {
        uint32_t frequency = song->notes[i].frequency;
        uint32_t duration = song->notes[i].duration;

        printf("note %u: %u Hz for %u ms\n", i, frequency, duration);
        play_sound(frequency);

        if (frequency == 0 || duration <= 35) {
            sleep_interrupt(duration);
        } else {
            sleep_interrupt(duration - 28);
            stop_sound();
            sleep_interrupt(28);
        }

        stop_sound();
    }

    disable_speaker();
    printf("finished song: %s\n", song->title);
}

void play_music(void)
{
    Song* songs[] = {
        (Song*)&brahms_op38_opening,
    };
    uint32_t song_count = sizeof(songs) / sizeof(songs[0]);
    SongPlayer* song_player = create_song_player();

    for (;;) {
        for (uint32_t i = 0; i < song_count; i++) {
            song_player->play_song(songs[i]);
            sleep_interrupt(1000);
        }
    }
}
