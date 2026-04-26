#include "libc/system.h"

void panic(const char* reason) {
    printf("KERNEL PANIC: %s\n", reason);
    asm volatile("cli");
    while(1) { asm volatile("hlt"); }
}
