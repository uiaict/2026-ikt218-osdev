#include "gdt.h"
#include "terminal.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "memory/kernel_memory.h"
#include "memory/paging.h"
#include "pit/pit.h"
#include <stdint.h>               // for uint32_t
#include "song_player/song_player.h"
#include "song_player/song.h"

extern uint32_t end; //to find out where the free memory begins
extern volatile uint32_t tick;
struct MyClass { int a; int b; };   //dummy class for testing new
extern void test_new(void);

void main(void)
{
    gdt_init();                // Set up Global Descriptor Table
    idt_init();                // Set up Interrupt Descriptor Table
    pic_remap();  
    init_pit();            // Remap PIC before setting IRQs
    asm volatile("sti");     // ENABLE INTERRUPTS
    
    terminal_write("Hello World! \n"); // Write to the screen

    
    init_kernel_memory(&end); // Initialize kernel memory manager
    init_paging(); // Setup paging
    print_memory_layout(); // See updated heap_end

    
    // Test malloc
    void* mem1 = malloc(1024);
    void* mem2 = malloc(2048);

    terminal_write("mem1: 0x");
    terminal_write_hex((uint32_t)mem1);
    terminal_write("\n");

    terminal_write("mem2: 0x");
    terminal_write_hex((uint32_t)mem2);
    terminal_write("\n");

    test_new(); // Test C++ new operator

    // Test song player
    terminal_write("\nStarting music...\n");
    play_music();
    terminal_write("Music finished.\n");


    // MAIN LOOP
    int counter = 0;

    while (1)
    {
        terminal_write("tick: ");
        terminal_write_dec(tick);
        terminal_write("\n");

        terminal_write("[");
        terminal_write_dec(counter);
        terminal_write("] Sleeping with busy-waiting (HIGH CPU)\n");

        sleep_busy(1000);

        terminal_write("[");
        terminal_write_dec(counter);
        terminal_write("] Slept using busy-waiting\n");
        counter++;

        terminal_write("[");
        terminal_write_dec(counter);
        terminal_write("] Sleeping with interrupts (LOW CPU)\n");

        sleep_interrupt(1000);

        terminal_write("[");
        terminal_write_dec(counter);
        terminal_write("] Slept using interrupts\n");
        counter++;
    }

}