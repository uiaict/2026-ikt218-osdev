#include "../gdt.h"
#include "../idt.h"
#include "../irq.h"
#include "../include/io.h"
#include "../include/print.h"
#include <libc/stdint.h>

#include "../keyboard.h"
#include "kernel/memory.h"
#include "kernel/pit.h"
#include <libc/stdio.h>
#include "song/song.h"

extern uint32_t end;

#define VGA_TEXT_BUFFER 0xb8000u
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_CTRL_PORT   0x3D4
#define VGA_DATA_PORT   0x3D5


static volatile uint16_t *vp = (uint16_t *)VGA_TEXT_BUFFER + 2;

void print_string(const char *s, uint8_t attrib)
{
	while (*s) {
		*vp = (attrib << 8) | *s++;
        vp++;
    }
    update_cursor();
}

void scroll() {
    volatile uint16_t *buf = (volatile uint16_t *)VGA_TEXT_BUFFER;
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
        buf[i] = buf[i + VGA_WIDTH];
    }
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) {
        buf[i] = (0x07 << 8) | ' ';
    }
    vp = buf + VGA_WIDTH * (VGA_HEIGHT - 1);
}

void print_newline() {
    // Find how far into the current line we are
    uint32_t current = vp - (volatile uint16_t *)VGA_TEXT_BUFFER;
    // Skip forward to the next multiple of 80
    vp += VGA_WIDTH - (current % VGA_WIDTH);

    // If we've gone past the last row, scroll instead of wrapping
    if (vp >= (volatile uint16_t *)VGA_TEXT_BUFFER + (VGA_WIDTH * VGA_HEIGHT)) {
        scroll();
        update_cursor();
    }

}
void update_cursor() {
    uint32_t pos = vp - (volatile uint16_t *)VGA_TEXT_BUFFER;
    
    outb(VGA_CTRL_PORT, 0x0F);
    outb(VGA_DATA_PORT, (uint8_t)(pos & 0xFF));
    outb(VGA_CTRL_PORT, 0x0E);
    outb(VGA_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));
}

void print_backspace() {
    vp--;
    *vp = (0x07 << 8) | ' ';
    update_cursor();
}

void play_song_impl(Song* song);

void kernel_main() {
    gdt_init();
    idt_init();   
    irq_init(); 
    keyboard_init();
    asm("sti");  // enable interrupts

    init_kernel_memory(&end); // heap starts just after the kernel binary ends in memory
    init_paging();            // map virtual addresses to physical - required for safe memory access
    print_memory_layout();    // print heap bounds so we can verify memory is set up correctly
    init_pit();               // start timer firing at 1000 Hz - each tick increments our counter by 1


    // print_string("Hello World ", 0x07); // outdated method pfffff
 //   printf("Hello World!\n");

    // Allocate three different sized blocks to verify malloc works
    //void* some_memory = malloc(12345);
    //void* memory2 = malloc(54321);
    //void* memory3 = malloc(13331);

//Commenting out assignment 4 interupts so they dont mess with anything
    //int counter = 0;
    //while(1) {
        // sleep_busy: keeps CPU in a loop checking the tick counter - wastes CPU cycles
    //    printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
      //  sleep_busy(1000);
        //printf("[%d]: Slept using busy-waiting.\n", counter++);
        //printf("test");

        // sleep_interrupt: halts CPU until next interrupt fires - CPU does nothing while waiting
    //    printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
      //  sleep_interrupt(1000);
        //printf("[%d]: Slept using interrupts.\n", counter++);
    //}

//    Song songs[] = {
//        {music_1, sizeof(starwars_theme) / sizeof(Note)}
  //  };
    //uint32_t n_songs = sizeof(songs) / sizeof(Song);

    //SongPlayer* player = create_song_player();

    //while(1) {
    //    for(uint32_t i = 0; i < n_songs; i++) {
    //        printf("Song starting...\n");
    //        player->play_song(&songs[i]);
    //        printf("Song finished.\n");
    //   }
    //}
    // Startup screen - imporvisation
    printf("----------------------------------------\n");
    printf("  CryptOS - XOR Encryption Terminal\n");
    printf("----------------------------------------\n");
    printf("\n");
    printf("XOR encryption works by combining each\n");
    printf("character with a secret key using the\n");
    printf("XOR operation. The same key decrypts it.\n");
    printf("\n");
    printf("Controls:\n");
    printf("  Ctrl+M - Mute/unmute music\n");
    printf("  Ctrl+N - Next song\n");
    printf("  Ctrl+P - Previous song\n");
    printf("  Please select a key before controlling music\n");
    printf("\n");
    printf("Music is playing. Type to encrypt!\n");
    printf("--------------------------------------\n");
    printf("\n");

    // get encryption key from user
    printf("Enter encryption key (one character): ");

    // Wait for a single keypress - read directly from keyboard port
    // spin until a valid ASCII key is pressed
    uint8_t key = 0;
    while (key == 0) {
        asm volatile("sti; hlt");
        // keyboard_handler prints the char - we need to also capture it
        key = get_last_key();
    }
    printf("\n");

    uint8_t xor_key = key;
    printf("Key set to: %c (0x%x)\n", xor_key, xor_key);
    printf("\n");
    printf("Now type your message and press Enter:\n");
    printf("\n");

    // Main encryption loop
    get_last_key(); //clear
    while (1) {
        printf("Plaintext : ");
        // collect input
        char buffer[80];
        uint8_t len = 0;
        uint8_t c = 0;

        get_last_key(); //clear so it doesnt bug out
        while (1) {
            asm volatile("sti; hlt");
            c = get_last_key();
            if (c == 0) continue;
            if (c == '\n') break;
            if (c == '\b' && len > 0) { len--; continue; }
            if (len < 79) buffer[len++] = c;
        }
        buffer[len] = 0;
        printf("\n");

        // Print encrypted version
        printf("Encrypted : ");
        for (uint8_t i = 0; i < len; i++) {
            uint8_t encrypted = buffer[i] ^ xor_key;
            // Print as hex since result may not be printable ASCII
            printf("%x ", encrypted);
        }
        printf("\n\n");
    }
}

