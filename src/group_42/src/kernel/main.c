#include <boot/multiboot2.h>
#include <kernel/panic.h>
#include <kernel/util.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "arch/i386/cpu/gdt.h"
#include "arch/i386/cpu/idt.h"
#include "drivers/input/keyboard.h"
#include "drivers/video/vga_terminal.h"
#include "kernel/log.h"
#include "kernel/syscall.h"
#include "kernel/util.h"
#include "shell/shell.h"


struct multiboot_info;
extern uint32_t end; // defined in arch/i386/linker.ld

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
  log_init();


  log_info("Initializing Group_42 Kernel...\n");

  // log_info("TSC: 0x%llx (%llu cycles)\n", rdtsc(), rdtsc());

  if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    kernel_panic("Invalid magic number 0x%x. Expected 0x%x\n", magic, MULTIBOOT2_BOOTLOADER_MAGIC);
  }
  init_gdt();
  init_idt();
  keyboard_set_scancode_set2();

  // init_mm(&end);
  // init_paging();
  // print_memory_layout();
  // init_pit();

  // test memory
  // TODO: make this work with new MM
  init_syscalls();

  log_info("Starting shell...\n");


  shell_init();
  shell_run();
}
