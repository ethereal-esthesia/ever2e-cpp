#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/macos}"
OUTPUT_BIN="${OUTPUT_BIN:-$BUILD_DIR/microcode_tests}"
CXX_BIN="${CXX_BIN:-c++}"

mkdir -p "$BUILD_DIR"

CXXFLAGS=(
  -std=c++17
  -O2
  -Wall
  -Wextra
  -I"$ROOT_DIR/include"
  -I"$ROOT_DIR/tests/microcode"
)
LDFLAGS=()

if pkg-config --exists sdl3; then
  CXXFLAGS+=( $(pkg-config --cflags sdl3) )
  LDFLAGS+=( $(pkg-config --libs sdl3) )
else
  echo "Error: SDL3 headers/libs not found." >&2
  exit 1
fi

CORE_SOURCES=()
while IFS= read -r file; do
  CORE_SOURCES+=("$file")
done < <(find "$ROOT_DIR/src" -name '*.cpp' -type f ! -name 'ever2e.cpp' | sort)
TEST_SOURCES=()
while IFS= read -r file; do
  TEST_SOURCES+=("$file")
done < <(find "$ROOT_DIR/tests/microcode" -name '*.cpp' -type f | sort)

"$CXX_BIN" "${CXXFLAGS[@]}" "${CORE_SOURCES[@]}" "${TEST_SOURCES[@]}" -o "$OUTPUT_BIN" "${LDFLAGS[@]}"
"$OUTPUT_BIN"
