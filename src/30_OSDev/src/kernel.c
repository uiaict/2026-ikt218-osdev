#include "gdt.h"
#include "terminal.h"


void main(void)
{
    gdt_init();                // Sett opp GDT
    terminal_write("Hello World\n"); // Skriv til skjermen
    while(1) { }               // Stop CPU 
}