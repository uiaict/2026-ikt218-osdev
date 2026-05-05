#include "libc/stdint.h"
#include "gdt.h"
#include "terminal.h"
#include "idt.h"
#include "irq.h"
#include "keyboard.h"
#include "kernel/memory.h"
#include "kernel/pit.h"
#include "kernel/pcspk.h"
#include "kernel/song.h"

extern uint32_t end;

struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    void* first;
};

void main(uint32_t magic, struct multiboot_info* mb_info) {
    // Init alt som før
    gdt_init();
    terminal_init();
    idt_init();
    irq_init();
    keyboard_init();
    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();
    init_pit();  // Viktig for sleep_busy()
    
    terminal_write("\n=== Music Player Ready ===\n");
    
    // Test: spill en kort tone
    // terminal_write("Testing speaker (A4=440Hz)...\n");
    // play_sound(440);
    // sleep_busy(500);
    // stop_sound();
    // terminal_write("Test done!\n");
    
    // Spill sanger i loop
    Song songs[] = {
        {music_1, sizeof(music_1) / sizeof(Note)},
        {starwars_theme, sizeof(starwars_theme) / sizeof(Note)}
    };
    uint32_t n_songs = sizeof(songs) / sizeof(Song);
    
    SongPlayer* player = create_song_player();
    
    while (1) {
        for (uint32_t i = 0; i < n_songs; i++) {
            terminal_write("\n>>> Playing next song <<<\n");
            player->play_song(&songs[i]);
            sleep_busy(1000);  // 1 sek pause
        }
        asm volatile("hlt");
    }
}