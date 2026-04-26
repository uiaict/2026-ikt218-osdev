#include "gdt/gdt.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/string.h"
#include "libc/stdio.h"
#include <multiboot2.h>
#include "idt/idt.h"

struct multiboot_info {
    uint32_t size;
    uint32_t reserved;
    struct multiboot_tag *first;
};

int kernel_main();

int main(uint32_t magic, struct multiboot_info* mb_info_addr) {
    (void)magic;
    (void)mb_info_addr;

    printf("Starting up...\n");
    
    idt_init();
    printf("IDT Starting");
    
    gdt_install();
    printf("[kernel] GDT installed\n");
    
    char str[] = "Hello, world!";

    print(str, strlen(str));
    putchar('\n');
    
    printf("[kernel] Video+serial output written, calling kernel_main()\n");

    //Interrupt test
    asm("int $0xE");

    // Call cpp kernel_main (defined in kernel.cpp)
    return kernel_main();
}