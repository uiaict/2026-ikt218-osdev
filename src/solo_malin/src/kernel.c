#include "gdt.h"

// VGS text mode buffer starts at physical address 0xB8000
// Each 16-bit entry: high byte = color attribute, low byte = ASCII character.
#define VGA ((unsigned short*)0xB8000)

//Simple function to write a null-terminated string to the top of the screen.
static void write_string(const char *s) {
    unsigned int i = 0;
    while (s[i]) {
        VGA[i] = 0x0F00 | (unsigned short)s[i];
        i++;
    }
}

// Kernel entry point
void main(void)
{
    gdt_init();     // Initialize and load GDT 
    write_string("Hello World!");       // Pint a test message to the VGA text buffer.

    //Halt the CPU in an infinite loop to keep the kernel running
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}