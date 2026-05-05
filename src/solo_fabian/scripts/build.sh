#!/usr/bin/env bash
set -e

GROUP="solo_fabian"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"

SRC="$ROOT/src/$GROUP"
BUILD="$ROOT/build/$GROUP"

cmake -S "$SRC" -B "$BUILD" \
  -DCMAKE_C_COMPILER=/usr/local/bin/i686-elf-gcc \
  -DCMAKE_CXX_COMPILER=/usr/local/bin/i686-elf-g++

cmake --build "$BUILD"
cmake --build "$BUILD" --target uiaos-create-image
