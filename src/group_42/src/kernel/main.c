#include <boot/multiboot2.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "arch/i386/cpu/gdt.h"
#include "arch/i386/cpu/idt.h"
#include "drivers/input/keyboard.h"
#include "drivers/video/vga_terminal.h"
#include "kernel/syscall.h"
#include "shell/shell.h"


struct multiboot_info;

void kernel_main(uint32_t magic, void* addr);

/// Entrypoint
/// @param magic multiboot magic number
/// @param addr  address of the multiboot_info struct
/// @return shouldn't return
int k_init(uint32_t magic, struct multiboot_info* addr) {
  kernel_main(magic, addr);

  // If kernel_main fails, disable interrupts and halt

  __asm__ volatile("cli");
  while (true) {
    __asm__ volatile("hlt");
  }
}


void kernel_main(uint32_t magic, void* addr) {
  vga_terminal_initialise();

  // TODO: Add boot timestamp

  // NOTE: Verify multiboot magic number, if it fails, then addr pointer is likely corrupted, so we
  // halt.
  if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    printf("Error: Invalid magic number 0x%x. Expected 0x%x\n", magic, MULTIBOOT2_BOOTLOADER_MAGIC);
    printf("Halting system.\n");
    return;
  }

  printf("Initializing Group_42 Kernel...\n");


  if (!init_gdt()) {
    printf("Failed to initialize GDT. Halting...\n");
    return;
  }


  printf("Initializing IDT...\n");

  init_idt();
  init_syscalls();
  init_keyboard();
  keyboard_set_scancode_set2();

  printf("Successfully initialized IDT.\n");


  printf("Keyboard scancode set 2 enabled.\n");

  shell_init();
  shell_run();
}
