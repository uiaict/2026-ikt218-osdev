#include "gdt/gdt.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/string.h"
#include <multiboot2.h>
#include "idt/idt.h"

struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

int kernel_main();

// Serial port output for debugging
static inline void serial_write_char(char c) {
    // Port 0x3F8 is COM1
    // Wait for transmit hold register to be empty
    while (!(*(volatile uint8_t*)0x3FD & 0x20));
    *(volatile uint8_t*)0x3F8 = c;
}

static void serial_write(const char* str) {
    while (*str) {
        if (*str == '\n') serial_write_char('\r');
        serial_write_char(*str++);
    }
}

int main(uint32_t magic, struct multiboot_info* mb_info_addr) {
    idt_init();
    serial_write("[kernel] Starting up...\n");
    
    gdt_install();
    serial_write("[kernel] GDT installed\n");
    
    char str[] = "Hello, world!";

    // Write to video mem (text mode at 0xB8000)
    char* video_mem = (char*)0xB8000;

    // Write "Hello, world!" to video memory with bright white on black
    for (size_t i = 0; i < strlen(str); i++) {
        video_mem[i * 2] = str[i]; // Character
        video_mem[i * 2 + 1] = 0x0F; // Bright white on black (was 0x07 = dim white)
    }
    
    serial_write("[kernel] Video output written, calling kernel_main()\n");

    asm("int $0x0");

    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}