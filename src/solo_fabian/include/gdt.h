#pragma once

/* Use the kernel's own fixed-width integer types instead of the host libc. */
#include <libc/stdint.h>

/*
 * The Global Descriptor Table tells the CPU what segment selectors mean.
 * This kernel uses a simple flat memory model:
 *   selector 0x00: required null descriptor
 *   selector 0x08: kernel code segment
 *   selector 0x10: kernel data segment
 */
/* The first real descriptor is entry 1, so its selector is 1 * 8 = 0x08. */
#define GDT_KERNEL_CODE_SELECTOR 0x08

/* The second real descriptor is entry 2, so its selector is 2 * 8 = 0x10. */
#define GDT_KERNEL_DATA_SELECTOR 0x10

/* Build the GDT in memory and ask assembly code to load it into the CPU. */
void gdt_init(void);
