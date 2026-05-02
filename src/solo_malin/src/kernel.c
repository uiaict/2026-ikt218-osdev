#include "gdt.h"
#include "idt.h"
#include "screen.h"
#include "keyboard.h"

// Kernel entry point
void main(void)
{
    gdt_init();     // Initialize and load GDT 
    write_string("GDT OK\n");       // Pint a test message to the VGA text buffer.

    idt_init();
    write_string("IDT OK\n");

    keyboard_init();
    write_string("keyboard initialized\n");

    __asm__ __volatile__("sti");
    write_string("Type on the keyboard: \n");

    //__asm__ __volatile__("int $0x3"); // for testing
    //__asm__ __volatile__("int $0x0"); // For testing
    //__asm__ __volatile__("int $0x4"); // For Testing

    //Halt the CPU in an infinite loop to keep the kernel running
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}