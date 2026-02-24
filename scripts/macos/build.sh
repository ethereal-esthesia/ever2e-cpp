#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/macos}"
OUTPUT_BIN="${OUTPUT_BIN:-$BUILD_DIR/ever2e}"
CXX_BIN="${CXX_BIN:-c++}"

mkdir -p "$BUILD_DIR"

CXXFLAGS=(
  -std=c++17
  -O2
  -Wall
  -Wextra
  -I"$ROOT_DIR/include"
)

LDFLAGS=()

if pkg-config --exists sdl3; then
  CXXFLAGS+=( $(pkg-config --cflags sdl3) )
  LDFLAGS+=( $(pkg-config --libs sdl3) )
else
  echo "Error: SDL3 headers/libs not found." >&2
  echo "Install SDL3 and ensure pkg-config is available." >&2
  exit 1
fi

SOURCES=( $(find "$ROOT_DIR/src" -name '*.cpp' -type f | sort) )

if [[ ${#SOURCES[@]} -eq 0 ]]; then
  echo "Error: no source files found under $ROOT_DIR/src" >&2
  exit 1
fi

echo "Building ever2e for macOS..."
"$CXX_BIN" "${CXXFLAGS[@]}" "${SOURCES[@]}" -o "$OUTPUT_BIN" "${LDFLAGS[@]}"
echo "Built: $OUTPUT_BIN"
