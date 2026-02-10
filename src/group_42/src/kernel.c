#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/stdio.h"
#include <multiboot2.h>

#include "vga_terminal.h"


void kernel_main(uint32_t magic, void* addr);

/// Entrypoint
/// @param magic multiboot magic number
/// @param addr  address of the multiboot_info struct
/// @return shouldnt return
int k_init(uint32_t magic, struct multiboot_info* addr) {

    kernel_main(magic, addr);

    // If kernel_main fails, disable interrupts and halt
    __asm__ volatile("cli");
    while (true) {
        __asm__ volatile("hlt");
    }
}



void kernel_main(uint32_t magic, void* addr){
    vga_terminal_initialize();

    // NOTE: Verify multiboot magic number, if it fails, then addr ponter is likely corrupted, so we halt.
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        printf("Error: Invalid magic number 0x%x. Expected 0x%x\n", magic, MULTIBOOT2_BOOTLOADER_MAGIC);
        printf("Halting system.\n");
        return;
    }

    printf("Kernel initialized successfully.\n");
    printf("Welcome to Group_42 Kernel!\n");
    fprintf(stderr, "SOme error\n");

    // parse multiboot info, to get memory map and kernel modules, or other drivers.

    // TODO: GDT

    // NOTE: TEST SOUND


    while (true) {
        __asm__ volatile("hlt");
    }
}
