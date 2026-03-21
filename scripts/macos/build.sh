#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/macos}"
APP_BIN="$BUILD_DIR/ever2e"
TEST_BIN="$BUILD_DIR/microcode_tests"
PRESET="${PRESET:-macos-release}"

if ! command -v cmake >/dev/null 2>&1; then
  echo "Error: cmake is required but was not found in PATH." >&2
  exit 1
fi

cmake --preset "$PRESET"
cmake --build --preset "$PRESET"
echo "Built: $APP_BIN"
echo "Built: $TEST_BIN"
