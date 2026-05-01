#include "terminal.h"
#include "colors.h"
#include "libc/stdint.h"
#include "kernel/memory.h"

extern uint32_t end;
#include "arch/i386/gdt.h"
#include "arch/i386/idt.h"
#include "arch/i386/isr.h"

int main() {
    init_gdt();
    terminal_write("Velkommen til FreDDaviDOS!", COLOR(YELLOW, BLUE), 0, 0);


    idt_init();

    // Ass 3 
    register_interrupt_handler(IRQ1, keyboard_callback);
    idt_enable_interrupts();


    terminal_write("IDT loaded", COLOR(WHITE, BLACK), 0, 1);

    asm volatile("int $0x3");

    terminal_write("After interrupt", COLOR(WHITE, BLACK), 0, 3);

    terminal_write("Keyboard input:", COLOR(WHITE, BLACK), 0, 9);

    
    terminal_write("Velkommen til FreDDaviDOS!", COLOR(YELLOW, BLUE), 0, 0);
    
    init_kernel_memory(&end);

    init_paging();

    print_memory_layout();

    void* some_memory = malloc(12345); 
    void* memory2 = malloc(54321); 
    void* memory3 = malloc(13331);

    while (1) { // coming soon 
        
    }

    return 0;
}