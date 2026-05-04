#include <../include/libc/stdio.h>
#include "../include/idt.h"
#include "../include/irq.h"
#include "../include/keyboard.h"
#include "../include/memory.h"
#include "../include/pit.h"
#include "../include/vga.h"
#include "../include/song.h"
#include "../include/matrix.h"
#include "../include/pic.h"

extern void idt_install(void);
extern void irq_install();
extern void keyboard_install();
extern uint8_t inb_port(uint16_t port);


//  Declared the C++ song player function so C can call it
extern void play_song_impl(Song* song);

// Random number generator for matrix
extern void srand(unsigned int seed);

// Print Menu 
void print_menu(void) {
    vga_clear();
    printf("================================================\n");
    printf("    UiA Operating System group 45 - Menu        \n");
    printf("================================================\n");
    printf("\n");
    printf("  1. Info & Tests\n");
    printf("  2. Play Song\n");
    printf("  3. Terminal\n");
    printf("  4. Matrix rain animation (pres 0 to return)\n");
    printf("\n");
    printf("================================================\n");
    printf("Select an option (1-4): ");
}

// Option 1: Info & Tests
 void run_info_and_tests(void) {
    // Page 1: Memory layout
    vga_clear();
    printf("==========================================\n");
    printf("         Info & Tests - Memory            \n");
    printf("==========================================\n\n");

    printf("--- Memory Layout ---\n");
    print_memory_layout();
    printf("\n");

    printf("--- Malloc Test ---\n");
    static void* some_memory = 0;
    static void* memory2     = 0;
    static void* memory3     = 0;
    if (some_memory == 0) {
        some_memory = malloc(12345);
        memory2     = malloc(54321);
        memory3     = malloc(13331);
    }
    printf("malloc(12345) = 0x%x\n", (unsigned int)some_memory);
    printf("malloc(54321) = 0x%x\n", (unsigned int)memory2);
    printf("malloc(13331) = 0x%x\n", (unsigned int)memory3); 

    printf("\nPress any key for sleep test...\n");
    keyboard_getchar();

    // Page 2: Sleep test
    int counter = 0;
    while (1) {
        vga_clear();
        printf("==========================================\n");
        printf("         Info & Tests - Sleep             \n");
        printf("==========================================\n\n");
        printf("--- Sleep & Interrupt Test ---\n");
        printf("Press 0 to return to menu.\n\n");

        printf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        printf("[%d]: Slept using busy-waiting.\n", counter++);

        printf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        printf("[%d]: Slept using interrupts.\n", counter++);

        char c = keyboard_getchar();
        if (c == '0') return;
    }
} 

// Option 2 - plays a Song 
void run_song(void) {
    vga_clear();
    printf("==========================================\n");
    printf("              Song Player                 \n");
    printf("==========================================\n");
    printf("\n");

    Song songs[] = {
        {music_1, sizeof(music_1) / sizeof(Note)}
    };

    printf("Playing Twinkle Twinkle Little Star...\n");
    printf("Press 0 after song ends to return to menu.\n\n");

    play_song_impl(&songs[0]);

    printf("\nDone! Press 0 to return to menu.\n");
    while (keyboard_getchar() != '0');
}

// Option 3: Terminal
 void run_terminal(void) {
    vga_clear();
    printf("==========================================\n");
    printf("           Write some text                \n");
    printf("==========================================\n");
    printf("Type anything. Press 0 to return.\n");
    printf("Not a real terminal, only for testing keyboard\n\n");
    
    // return to menu 
    while (1) {
        char c = keyboard_getchar();
        if (c == '0') return;
    }
} 

//  Main Menu Loop 
void run_menu(void) {
    while (1) {
        print_menu();
        char choice = keyboard_getchar();
        
        switch (choice) {
            case '1':
                run_info_and_tests();
                break;
            case '2':
                run_song();
                break;
            case '3':
                run_terminal();
                break;
            case '4':
                run_matrix_rain();
                break;
            default:
                // re display the menu
                break;
        }
    }
}

// Kernel Main function
void kernel_main(void) {

    // Initialize memory first
    init_kernel_memory(&end);

    // IDT and IRQ initialization
    idt_install();
    irq_install();
    keyboard_install();

    // Initialize paging
    init_paging();

    // Initialize PIT
    init_pit();

    // Flush PS2 buffer before enabeling the interrupts
    while (inb_port(0x64) & 0x01) inb_port(0x60);

    // Enable interrupts
    asm volatile("sti");

    // Flush keypresses during boot
    sleep_interrupt(200);


    // making Seed for the random number generator
    srand(0xDEADBEEF);

    // Start the menu
    run_menu();

    // Should never reach here
    while (1) {
        asm volatile("hlt");
    }
}

int main(void) {
    kernel_main();
    return 0;
}