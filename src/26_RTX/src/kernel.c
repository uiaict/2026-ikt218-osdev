#include <libc/stdbool.h>
#include <libc/stdint.h>
#include <libc/stdio.h>
#include <idt.h>
#include <kernel/memory.h>
#include <kernel/pit.h>
#include <kernel/common.h>

extern uint32_t end;

// === Song structures (matching song.h) ===
typedef struct {
    uint32_t frequency;
    uint32_t duration;
} Note;

typedef struct {
    Note* notes;
    uint32_t length;
} Song;

typedef struct {
    void (*play_song)(Song* song);
} SongPlayer;

// Defined in song.cpp
extern SongPlayer* create_song_player();
extern void play_song_impl(Song* song);

// Song data (from song.h frequencies)
#define R 0
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define E5 659
#define F5 698
#define G5 784
#define A5 880
#define G3 196
#define A3 220
#define B3 247
#define Gs5 831
#define A_SHARP4 466
#define G_SHARP4 415

static Note music_1[] = {
    {E5, 250}, {R, 125}, {E5, 125}, {R, 125}, {E5, 125}, {R, 125},
    {C5, 125}, {E5, 125}, {G5, 125}, {R, 125}, {G4, 125}, {R, 250},
    {C5, 125}, {R, 250}, {G4, 125}, {R, 125}, {E4, 125}, {R, 125},
    {A4, 125}, {B4, 125}, {R, 125}, {A_SHARP4, 125}, {A4, 125}, {R, 125},
    {G4, 125}, {E5, 125}, {G5, 125}, {A5, 125}, {F5, 125}, {G5, 125},
    {R, 125}, {E5, 125}, {C5, 125}, {D5, 125}, {B4, 125}, {R, 125},
    {C5, 125}, {R, 250}, {G4, 125}, {R, 125}, {E4, 125}, {R, 125},
    {A4, 125}, {B4, 125}, {R, 125}, {A_SHARP4, 125}, {A4, 125}, {R, 125},
    {G4, 125}, {E5, 125}, {G5, 125}, {A5, 125}, {F5, 125}, {G5, 125},
    {R, 125}, {E5, 125}, {C5, 125}, {D5, 125}, {B4, 125}, {R, 125},
};

static Note music_2[] = {
    {E4, 200}, {E4, 200}, {F4, 200}, {G4, 200}, {G4, 200}, {F4, 200}, {E4, 200}, {D4, 200},
    {C4, 200}, {C4, 200}, {D4, 200}, {E4, 200}, {E4, 400}, {R, 200},
    {D4, 200}, {D4, 200}, {E4, 200}, {F4, 200}, {F4, 200}, {E4, 200}, {D4, 200}, {C4, 200},
    {A4, 200}, {A4, 200}, {A4, 200}, {G4, 400},
};

void main(uint32_t magic, uint32_t mb_addr)
{
    (void)mb_addr;

    terminal_clear();
    terminal_set_color(VGA_COLOR_LCYAN);
    printf("===  UiAOS - 26_RTX  ===\n\n");
    terminal_set_color(VGA_COLOR_BWHITE);

    if (magic != 0x36d76289) {
        terminal_set_color(VGA_COLOR_LRED);
        printf("[ERROR] Not loaded by a Multiboot2 bootloader (magic=0x%x)\n", magic);
        for (;;) __asm__ volatile ("hlt");
    }

    // === IDT ===
    printf("[IDT] Setting up Interrupt Descriptor Table...\n");
    idt_init();
    printf("[IDT] Loaded (256 entries, ISRs 0-31, IRQs 0-15).\n");

    // === Memory ===
    printf("[MEM] Initializing kernel memory manager...\n");
    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();

    // === Memory allocation test ===
    void *mem1 = malloc(12345);
    void *mem2 = malloc(54321);
    void *mem3 = malloc(13331);
    printf("[MEM] malloc test: mem1=0x%x mem2=0x%x mem3=0x%x\n",
           (uint32_t)mem1, (uint32_t)mem2, (uint32_t)mem3);
    (void)mem1; (void)mem2; (void)mem3;

    // === PIT ===
    printf("[PIT] Initializing at 1000 Hz...\n");
    init_pit();
    printf("[PIT] PIT ready.\n");

    // Enable interrupts
    __asm__ volatile ("sti");

    printf("[PIT] Testing busy sleep 250 ms...\n");
    sleep_busy(250);
    printf("[PIT] Busy sleep OK. Testing interrupt sleep 250 ms...\n");
    sleep_interrupt(250);
    printf("[PIT] Interrupt sleep OK.\n");

    // === Assignment 5: Music Player ===
    printf("[MUSIC] Starting PC speaker music test...\n");

    Song songs[] = {
        {music_1, sizeof(music_1) / sizeof(Note)},
        {music_2, sizeof(music_2) / sizeof(Note)},
    };
    uint32_t n_songs = sizeof(songs) / sizeof(Song);

    SongPlayer* player = create_song_player();

    for (uint32_t i = 0; i < n_songs; i++) {
        printf("[MUSIC] Playing song %d/%d...\n", i + 1, n_songs);
        player->play_song(&songs[i]);
        printf("[MUSIC] Finished song %d.\n", i + 1);
        sleep_interrupt(500);
    }
    printf("[MUSIC] Music test completed.\n");

    // === Keyboard ready ===
    printf("[KB] Keyboard ready. Type something:\n");

    while (1) {
        __asm__ volatile ("sti\n\thlt");
    }
}
