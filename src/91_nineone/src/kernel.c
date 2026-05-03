#include "terminal.h"
#include "menu.h"
#include "colors.h"
#include "libc/stdint.h"
#include "kernel/memory.h"
#include "libc/stdio.h"

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
    printf_color(COLOR(YELLOW, BLUE), "Velkommen til FreDDaviDOS!");

    idt_init();

    register_interrupt_handler(IRQ1, keyboard_callback);
    init_pit();

    idt_enable_interrupts();

    init_kernel_memory(&end);

    init_paging();

    print_memory_layout();

    init_pit();

    printf("Sleeping busy...");
    sleep_busy(10);
    printf("Done busy sleep");

    printf("Sleeping interrupt...");
    sleep_interrupt(10);

    printf("Done interrupt sleep");

    printf("IDT loaded");

    asm volatile("int $0x3");

    printf("After interrupt");

    printf("Keyboard input:");   

    printf("IDT loaded");

    asm volatile("int $0x3");

    printf("After interrupt");

    printf("Keyboard input:");

    void* some_memory = malloc(12345); 
    void* memory2 = malloc(54321); 
    void* memory3 = malloc(13331);
    menu_init();

    

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