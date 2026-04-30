#include "libc/stdint.h"
#include "terminal.h"
#include "gdt.h"
#include "idt.h"
#include "io.h"
#include "irq.h"
#include "isr.h"
#include "keyboard.h"
#include "memory.h"
#include "pit.h"

extern uint32_t end; // This is defined in arch/i386/linker.ld

void kmain(uint32_t magic, void* mb_info_addr) {
    (void)magic;
    (void)mb_info_addr;

    printf("Installing GDT...\n");
    init_gdt();
    terminal_initialize();

    printf("Installing IDT...\n");
    idt_install();

    printf("Installing IRQ...\n");
    irq_install();

    printf("Initializing PIT...\n");
    pit_init();

    init_kernel_memory(&end);
    init_paging();
    print_memory_layout();
    printf("Hello World!\n");
    void* some_memory = malloc(12345); 
    void* memory2 = malloc(54321); 
    void* memory3 = malloc(13331);

    printf("Registering keyboard handler...\n");
    irq_install_handler(1, keyboard_handler);

    // Also ensure keyboard itself is enabled
    outb(0x64, 0xAE);
    printf("Keyboard controller enabled.\n");
    printf("Enabling interrupts...\n");
    __asm__ __volatile__("sti");

    printf("\n=== OS Ready ===\n");
    printf("Type on your keyboard. Characters will appear below:\n");
    printf("----------------------------------------\n");

    uint32_t counter = 0;

    printf("[");
    print_int(counter); // Bruk din eksisterende funksjon
    printf("]: Sleeping with busy-waiting (HIGH CPU).\n");
        
    sleep_busy(1000);
        
    printf("[");
    print_int(counter++);
    printf("]: Slept using busy-waiting.\n");

    printf("[");
    print_int(counter);
    printf("]: Sleeping with interrupts (LOW CPU).\n");
        
    sleep_interrupt(1000);
        
    printf("[");
    print_int(counter++);
    printf("]: Slept using interrupts.\n");
    
    while (1) {
        __asm__ __volatile__("hlt");
    }
}