# Kernel Overview

This kernel runs in 32-bit protected mode and demonstrates a minimal setup with:

- Global Descriptor Table (GDT) with three entries:
	- Null descriptor
	- Kernel code segment (selector 0x08)
	- Kernel data segment (selector 0x10)
- GDT load via `lgdt`
- Segment register reload (`cs`, `ds`, `es`, `fs`, `gs`, `ss`)
- Basic text output to VGA text memory through a small terminal interface

## Files

- `gdt.c`: builds GDT entries and pointer, then calls `gdt_flush`
- `gdt.asm`: executes `lgdt`, reloads segment registers, and performs far jump for `cs`
- `terminal.c`: implements `putchar`, `print`, and a basic `printf`
- `kernel.c`: initializes GDT and prints `HELLO WORLD`
