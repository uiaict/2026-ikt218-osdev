# 77_lindestad

This directory contains the kernel submission for `77_lindestad`.

The parent repository README describes the course repository layout and the
recommended devcontainer setup. This file only documents the commands used to
build and run this project directly.

## Tested Environment

The project has been built and tested on Fedora Linux. The same commands should
also work from a Linux-like environment such as WSL, provided the required tools
are installed and available in `PATH`.

Required tools:

- `cmake`
- `ninja`
- `nasm`
- `i686-elf-gcc` and `i686-elf-g++`
- `xorriso`
- `mtools`
- `qemu-system-i386`
- Limine files installed under `/usr/local/limine`

The CMake image target expects Limine at `/usr/local/limine`.

## Build

Run these commands from the repository root:

```sh
cmake -S src/77_lindestad -B build/77_lindestad -G Ninja
cmake --build build/77_lindestad --target uiaos-create-image
```

The bootable ISO is written to:

```text
build/77_lindestad/kernel.iso
```

The final submitted build artifacts are:

```text
build/77_lindestad/kernel.bin
build/77_lindestad/kernel.iso
build/77_lindestad/disk.iso
```

`kernel.iso` is the bootable image used by QEMU. `disk.iso` is also included
because the course web/deploy helper expects each group build directory to
contain both `kernel.iso` and `disk.iso`.

## Run in QEMU

Run the ISO with:

```sh
qemu-system-i386 \
  -cdrom build/77_lindestad/kernel.iso \
  -m 128M \
  -audiodev pipewire,id=snd0 \
  -machine pcspk-audiodev=snd0
```

The `pipewire` audio backend is the preferred local choice on Fedora. If QEMU
was built without PipeWire audio support, try `-audiodev pa,id=snd0` or
`-audiodev sdl,id=snd0`. For silent automated testing, use
`-audiodev none,id=snd0`.

## Current Demo

The current kernel starts the assignment 6 Bomberman-lite demo after the earlier
kernel initialization checks complete.

The goal is to defeat every enemy. Destroying crates gives score and opens the
level, but the win screen appears when all enemies are dead.

Controls:

- `w`, `a`, `s`, `d`: move
- space: place bomb
- `r`: restart after game over or victory
- `q`: toggle speaker output

The game uses VGA text output, buffered keyboard input, PIT timing, heap
allocation, background PC speaker music, and PC speaker sound effects.

## Debug Helper

`scripts/start_qemu.sh` starts QEMU paused with a GDB server and waits for
`gdb-multiarch`. It is intended for debugging, not for the normal quick-run
path above.
