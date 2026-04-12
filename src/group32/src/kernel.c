#include "gdt.h"

void kernel_main(void) {
    unsigned short* video_memory = (unsigned short*) 0xB8000;
    const char* message = "Hello World";

    gdt_install();

    int i = 0;
    while (message[i] != '\0') {
        video_memory[i] = (unsigned short)message[i] | (0x0F << 8);
        i++;
    }

    while (1) {
    }
}