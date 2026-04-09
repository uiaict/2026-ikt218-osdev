# Project Guide

This file is a quick introduction to the active `3_PirateOS` kernel for anyone who
is reading the project for the first time.

## What This Project Is

This repository contains a small educational x86 operating system kernel written
 in C, C++, and a little assembly. It boots through Multiboot, sets up the CPU
descriptor tables and interrupt handling, enables paging, provides a simple heap,
offers a text terminal with a CLI, and includes a few user-visible demo systems
such as a PC-speaker music player and an ASCII raycaster game.

The project is intentionally simple rather than fully general. Many subsystems
solve only the current kernel's needs, but the structure is organized so that
new features can be added without rewriting everything first.

## Source Layout

- `src/kernel/`
  Kernel-level services that are not themselves memory managers or interrupt
  drivers. Right now this contains the CLI and PIT code.
- `src/memory/`
  Heap and paging implementations.
- `memory/`
  Public headers for the memory subsystem.
- `src/interrupts/`
  IDT setup, ISR/IRQ dispatch, keyboard IRQ handling, and interrupt assembly
  stubs.
- `include/interrupts/`
  Public interrupt-related headers.
- `src/libc/` and `include/libc/`
  Small freestanding libc-style helpers such as `printf`, `memset`, and types.
- `src/apps/` and `include/apps/`
  Demo applications and their headers. The most substantial app is the
  raycaster.
- `src/arch/`
  Architecture-specific linker scripts.
- `scripts/`
  Helper scripts for local development and QEMU boot.

## Boot Flow

The boot path starts in assembly and then hands off to `main()` in
`src/kernel.c`.

The important initialization order is:

1. terminal setup
2. GDT
3. IDT
4. ISR/IRQ setup
5. heap setup
6. paging setup
7. keyboard setup
8. PIT setup
9. enabling CPU interrupts

After that, the kernel enters a simple main loop in `src/kernel.cpp`.

## Core Subsystems

### Memory

The memory layer has two parts:

- `malloc/free` for normal variable-sized allocations
- `pmalloc/pfree` for page-aligned runs used by paging-sensitive code

The current heap is a simple first-fit allocator with block headers and
coalescing of adjacent free blocks.

### Interrupts

The interrupt layer is split across:

- `idt.c` for the interrupt descriptor table
- `isr.c` for exception/IRQ registration and PIC remapping
- `interrupts.asm` for low-level stubs
- `keyboard.c` for keyboard IRQ handling

Drivers such as PIT and keyboard register callbacks into the generic IRQ layer.

### Terminal and CLI

The text terminal lives mainly in `src/libc/stdio.c`.

The keyboard driver translates input and builds terminal lines, while
`src/kernel/cli.c` owns command execution and history. This separation matters:
the keyboard file is a driver, while the CLI file is a user-facing shell layer.

### PIT

The PIT driver in `src/kernel/pit.c` configures channel 0 as the kernel timer.
This gives the kernel a tick counter and two sleep helpers.

Channel 2 is left available for PC-speaker sound.

### Applications

There are currently two notable demo applications:

- a song player using the PC speaker
- a raycaster game using VGA mode 13h

These are useful not only as demos, but also as integration tests for the rest
of the kernel: input, timing, rendering, and sound all exercise the platform in
real time.

## Reading Order For New Contributors

If you are new to the project, this order usually works well:

1. `PROJECT_GUIDE.md`
2. `src/kernel.c`
3. `src/kernel.cpp`
4. `src/interrupts/isr.c` and `src/interrupts/keyboard.c`
5. `src/memory/heap.c` and `src/memory/paging.c`
6. `src/libc/stdio.c`
7. `src/kernel/cli.c`
8. the app you care about (`song` or `raycaster`)

## Design Philosophy

The project favors:

- clear ownership of modules over clever abstractions
- simple kernel-specific solutions over premature generalization
- visible runtime behavior for debugging
- incremental cleanup instead of large rewrites

That means some code is intentionally direct and hardware-near. The goal is not
to look like a production kernel yet, but to stay understandable while growing.
