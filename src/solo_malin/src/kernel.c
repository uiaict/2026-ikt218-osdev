#include "gdt.h"
#include "idt.h"
#include "screen.h"
#include "keyboard.h"
#include "memory.h"
#include "pit.h"
#include "song.h"
#include "menu.h"
#include <libc/stdint.h>

extern uint32_t end;

static void show_startup_info(void) {
    clearTerminal();
    write_string("GDT OK\n");
    write_string("IDT OK\n");
    write_string("Kernel memory initialized\n");
    write_string("Paging initialized\n");
    print_memory_layout();
    write_string("keyboard initialized\n");
    write_string("PIT ready\n");
    write_string("\nPress any key to continue to menu...\n");

    while (!keyboard_has_char()) {
        __asm__ __volatile__("hlt");
    }
    keyboard_get_char();
}

// Kernel entry point
void main(uint32_t magic, void* mb_info_addr)
{
    (void)magic;
    (void)mb_info_addr;

    gdt_init();     // Initialize and load GDT 
    idt_init();     // Initialize and load IDT
    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();
    keyboard_init();
    init_pit();

    /*
    void* some_memory = malloc(12345);
    void* memory2 = malloc(54321);
    void* memory3 = malloc(13331);

    if (some_memory && memory2 && memory3){
        write_string("malloc OK\n");
    }
    */

    __asm__ __volatile__("sti");
    write_string("Starting menu... \n");

    show_startup_info();
    run_main_menu();

    // Test loop for PIT sleeps (replaces the old hlt loop)
    /*
    int counter = 0;
    while (1) {
        kprintf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        kprintf("[%d]: Slept using busy-waiting.\n", counter++);

        kprintf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        kprintf("[%d]: Slept using interrupts.\n", counter++);
    }
    */

    //Halt the CPU in an infinite loop to keep the kernel running
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}