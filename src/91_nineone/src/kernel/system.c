#include "kernel/system.h"
#include "libc/stdint.h"
#include "../colors.h"

#define print terminal_write

void panic(const char* message)
{
    uint8 c = COLOR(WHITE, RED);

    print("KERNEL PANIC: ", c, 40, 0);
    print(message, c, 54, 0);

    while (1) {
        __asm__ __volatile__("hlt");
    }
}