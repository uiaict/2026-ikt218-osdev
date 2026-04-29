#include "../terminal.h"

void isr_handler(int interrupt)
{
    terminal_write("Interrupt number: ");

    if (interrupt == 0)
        terminal_write("0\n");
    else if (interrupt == 1)
        terminal_write("1\n");
    else if (interrupt == 2)
        terminal_write("2\n");
}