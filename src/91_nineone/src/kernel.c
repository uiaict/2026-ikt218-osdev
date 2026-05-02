#include "terminal.h"
#include "menu.h"
#include "colors.h"
#include "libc/stdint.h"
#include "kernel/memory.h"

extern uint32_t end;
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "arch/i386/isr.h"
#include "pit.h"

#include "Music_Player/song.h"

void play_music(void);
void play_song_impl(Song* song);

int main() {
    
    init_gdt();
    terminal_write("Velkommen til FreDDaviDOS!", COLOR(YELLOW, BLUE), 1, 1);

    idt_init();

    register_interrupt_handler(IRQ1, keyboard_callback);
    init_pit();

    idt_enable_interrupts();

    init_pit();

    terminal_write("Sleeping busy...", COLOR(WHITE, BLACK), 1, 7);
    sleep_busy(10);
    terminal_write("Done busy sleep", COLOR(WHITE, BLACK), 1, 8);

    terminal_write("Sleeping interrupt...", COLOR(WHITE, BLACK), 1, 9);
    sleep_interrupt(10);

    terminal_write("Done interrupt sleep", COLOR(WHITE, BLACK), 1, 10);



    terminal_write("IDT loaded", COLOR(WHITE, BLACK), 1, 11);

    asm volatile("int $0x3");

    terminal_write("After interrupt", COLOR(WHITE, BLACK), 1, 12);

    terminal_write("Keyboard input:", COLOR(WHITE, BLACK), 1, 13);
    
    init_kernel_memory(&end);

    init_paging();

    print_memory_layout();

    void* some_memory = malloc(12345); 
    void* memory2 = malloc(54321); 
    void* memory3 = malloc(13331);
    draw();

    play_music();

    while (1) {
    }
    return 0;
}

void play_music(void) {
    Song songs[] = {
        {music_1, sizeof(music_1) / sizeof(Note)}
    };

    uint32_t n_songs = sizeof(songs) / sizeof(Song);

    while (1) {
        for (uint32_t i = 0; i < n_songs; i++) {
            play_song_impl(&songs[i]);
        }
    }
}