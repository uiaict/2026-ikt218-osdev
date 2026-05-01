#include "terminal.h"
#include "colors.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"

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

    
    while (1) { // coming soon 
        __asm__ volatile("hlt");
    }

    return 0;
}