#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

ISO_PATH="${1:-${BUILD_DIR}/kernel.iso}"
MEMORY_MB="${QEMU_MEMORY_MB:-64}"

if ! command -v qemu-system-i386 >/dev/null 2>&1; then
    echo "Error: qemu-system-i386 is not installed or not on PATH."
    exit 1
fi

if [ ! -f "${ISO_PATH}" ]; then
    echo "Error: ISO image not found: ${ISO_PATH}"
    echo "Build it with: cmake --build ${BUILD_DIR} --target uiaos-create-image"
    exit 1
fi

echo "Starting QEMU with ${ISO_PATH}"
exec qemu-system-i386 -cdrom "${ISO_PATH}" -boot d -m "${MEMORY_MB}"
