#include "terminal.h"
#include "colors.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "pit.h"

int main() {
    
    init_gdt();
    terminal_write("Velkommen til FreDDaviDOS!", COLOR(YELLOW, BLUE), 0, 0);

    idt_init();

    register_interrupt_handler(IRQ1, keyboard_callback);
    init_pit();

    idt_enable_interrupts();

    init_pit();

    terminal_write("Sleeping busy...", COLOR(WHITE, BLACK), 0, 4);
    sleep_busy(10);
    terminal_write("Done busy sleep", COLOR(WHITE, BLACK), 0, 5);

    terminal_write("Sleeping interrupt...", COLOR(WHITE, BLACK), 0, 6);
    sleep_interrupt(10);

    terminal_write("Done interrupt sleep", COLOR(WHITE, BLACK), 0, 7);



    terminal_write("IDT loaded", COLOR(WHITE, BLACK), 0, 1);

    asm volatile("int $0x3");

    terminal_write("After interrupt", COLOR(WHITE, BLACK), 0, 3);

    terminal_write("Keyboard input:", COLOR(WHITE, BLACK), 0, 9);

    
    while (1) { // coming soon 
        __asm__ volatile("hlt");
    }

    return 0;
}