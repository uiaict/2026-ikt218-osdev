# Group 45 OSDEV

32-bit operating system built with a custom Limine bootloader, booting into a simple menu.

---

## src directory Structure

```
src/
├── arch/               
│   ├── i386/           # 32-bit x86 linker script
│   └── x86_64/         # 64-bit x86 linker script
├── libc/               # Minimal C standard library implementation
│   └── stdio.c         # printf and basic I/O
├── gdt.asm             # Global Descriptor Table setup (assembly)
├── idt.asm             # IDT load routine (assembly)
├── idt.c               # Interrupt Descriptor Table initialization
├── irq.asm             # IRQ stubs
├── irq.c               # IRQ dispatcher and spurious IRQ handling
├── isr.c               # Interrupt Service Routine handlers
├── kernel.c            # Kernel entry point and main menu
├── keyboard.c          # PS/2 keyboard driver with ring buffer
├── malloc.c            # Simple heap allocator
├── matrix.c            # Matrix rain animation
├── memory.c            # Memory layout and paging 
├── memutils.c          # memset/memcpy utilities
├── multiboot2.asm      # Multiboot2 header
├── pic.c               # PIC driver
├── pit.c               # PIT timer driver
├── songplayer.cpp      # PC speaker song player (C++)
└── vga.c               # simple VGA driver 
```

---

## Components

**Bootloader**
- Limine v10.8.5 is used as the bootloader, configured via `limine.conf` 

**Drivers**
- `keyboard.c` — scancode translation, ring buffer, backspace and echo handling
- `vga.c` — direct VRAM writes at `0xB8000`, cursor tracking, scrolling, backspace
- `pit.c` — configures the 8253/8254 timer, provides `sleep_busy` and `sleep_interrupt`
- `pic.c` — PIC initialization, EOI signaling

**Memory**
- `memory.c` sets up a simple heap and initializes paging
- `malloc.c` provides `malloc` and `free` 

**Menu Options**
1. **Info & Tests** — memory layout, malloc demonstration, sleep test
2. **Song Player** — plays a melody via the PC speaker
3. **Terminal** — simple text input with backspace support, exit using 0
4. **Matrix Rain** — cool animated falling characters

---

## Running with sound from build directory

```bash
qemu-system-i386 -audiodev pa,id=snd -machine pcspk-audiodev=snd kernel.iso
```