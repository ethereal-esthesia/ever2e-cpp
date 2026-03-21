#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/macos}"
OUTPUT_BIN="${OUTPUT_BIN:-$BUILD_DIR/microcode_tests}"
REBUILD_ON_RUN="${REBUILD_ON_RUN:-1}"
PRESET="${PRESET:-macos-release}"

if [[ "$REBUILD_ON_RUN" == "1" || ! -x "$OUTPUT_BIN" ]]; then
  "$ROOT_DIR/scripts/macos/build.sh"
fi

if command -v ctest >/dev/null 2>&1; then
  ctest --preset "$PRESET" -R '^microcode_tests$'
else
  "$OUTPUT_BIN"
fi
