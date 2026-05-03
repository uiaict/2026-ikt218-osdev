#include "gdt.h"
#include "idt.h"
#include "screen.h"
#include "keyboard.h"
#include <libc/stdint.h>
#include "memory.h"
#include "pit.h"

extern uint32_t end;

// Kernel entry point
void main(uint32_t magic, void* mb_info_addr)
{

    gdt_init();     // Initialize and load GDT 
    write_string("GDT OK\n");       // Pint a test message to the VGA text buffer.

    idt_init();
    write_string("IDT OK\n");


    init_kernel_memory(&end);
    write_string("Kernel memory initialized\n");

    init_paging();
    write_string("Paging initialized\n");

    print_memory_layout();

    keyboard_init();
    write_string("keyboard initialized\n");

    void* some_memory = malloc(12345);
    void* memory2 = malloc(54321);
    void* memory3 = malloc(13331);

    if (some_memory && memory2 && memory3){
        write_string("malloc OK\n");
    }

    init_pit();
    write_string("PIT ready\n");

    __asm__ __volatile__("sti");
    write_string("Type on the keyboard: \n");

    // Test loop for PIT sleeps (replaces the old hlt loop)
    int counter = 0;
    while (1) {
        kprintf("[%d]: Sleeping with busy-waiting (HIGH CPU).\n", counter);
        sleep_busy(1000);
        kprintf("[%d]: Slept using busy-waiting.\n", counter++);

        kprintf("[%d]: Sleeping with interrupts (LOW CPU).\n", counter);
        sleep_interrupt(1000);
        kprintf("[%d]: Slept using interrupts.\n", counter++);
    }



    //Halt the CPU in an infinite loop to keep the kernel running
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}