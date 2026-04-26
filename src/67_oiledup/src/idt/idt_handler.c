#include "libc/stdio.h"
#include "../keyboard/keyboard.h"

void interrupt_handler(int vector)
{
    switch (vector) {
        case 0:
            printf("Interrupt 0");
            break;
        case 8:
            printf("Interrupt 8");
            break;
        case 14:
            printf("Interrupt 14");
            break;
        case 32:
            break;
        case 33:
            keyboard_handler();
            break;
    }
}