#!/usr/bin/env bash
set -e

WORKSPACE="/workspaces/2026-ikt218-osdev"
SRC_DIR="$WORKSPACE/src/group_29"
BUILD_DIR="$WORKSPACE/build/group_29"

"$SRC_DIR/scripts/build.sh"

echo "Launching QEMU without debugger... close the QEMU window or press Ctrl+C to stop."
qemu-system-i386 \
  -boot d \
  -hda "$BUILD_DIR/kernel.iso" \
  -hdb "$BUILD_DIR/disk.iso" \
  -m 64 \
  -audiodev sdl,id=sdl1,out.buffer-length=40000 \
  -machine pcspk-audiodev=sdl1 \
  -serial pty