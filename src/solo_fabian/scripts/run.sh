#!/usr/bin/env bash
set -e

GROUP="solo_fabian"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"

bash "$ROOT/src/$GROUP/scripts/start_qemu.sh" \
  "$ROOT/build/$GROUP/kernel.iso" \
  "$ROOT/build/$GROUP/disk.iso"
