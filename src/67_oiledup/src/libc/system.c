#include "libc/system.h"

void panic(const char* reason) {
    printf("KERNEL PANIC: %s\n", reason);
    asm volatile("cli");
    while(1) { asm volatile("hlt"); }
}

void outb(unsigned short port, unsigned char val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
