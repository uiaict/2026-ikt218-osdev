#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
ISO_PATH="${1:-$PROJECT_ROOT/build/31_group31/kernel.iso}"

if [ ! -f "$ISO_PATH" ]; then
    echo "ISO not found: $ISO_PATH"
    echo "Run: cmake --build $PROJECT_ROOT/build/31_group31"
    exit 1
fi

AUDIODEV="sdl,id=audio0,out.buffer-length=40000"

if [ -S /mnt/wslg/PulseServer ]; then
    AUDIODEV="pa,id=audio0,server=/mnt/wslg/PulseServer"
elif [ -n "$PULSE_SERVER" ]; then
    AUDIODEV="pa,id=audio0,server=$PULSE_SERVER"
fi

echo "Booting $ISO_PATH"
echo "Using QEMU audio: $AUDIODEV"

exec qemu-system-i386 \
    -cdrom "$ISO_PATH" \
    -boot d \
    -m 64 \
    -audiodev "$AUDIODEV" \
    -machine pcspk-audiodev=audio0
