#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/macos}"
APP_BIN="$BUILD_DIR/ever2e"
TEST_BIN="$BUILD_DIR/microcode_tests"
PRESET="${PRESET:-macos-release}"
QUIET_BUILD="${QUIET_BUILD:-0}"
BUILD_TARGETS="${BUILD_TARGETS:-ever2e microcode_tests}"

if ! command -v cmake >/dev/null 2>&1; then
  echo "Error: cmake is required but was not found in PATH." >&2
  exit 1
fi

read -r -a TARGETS <<< "$BUILD_TARGETS"

if [[ "${#TARGETS[@]}" -eq 0 ]]; then
  echo "Error: BUILD_TARGETS resolved to empty target list." >&2
  exit 1
fi

if [[ "$QUIET_BUILD" == "1" ]]; then
  BUILD_LOG="$(mktemp)"
  trap 'rm -f "$BUILD_LOG"' EXIT
  if ! cmake --preset "$PRESET" >"$BUILD_LOG" 2>&1; then
    cat "$BUILD_LOG"
    exit 1
  fi
  if ! cmake --build --preset "$PRESET" --target "${TARGETS[@]}" >>"$BUILD_LOG" 2>&1; then
    cat "$BUILD_LOG"
    exit 1
  fi
else
  cmake --preset "$PRESET"
  cmake --build --preset "$PRESET" --target "${TARGETS[@]}"
fi

for target in "${TARGETS[@]}"; do
  case "$target" in
    ever2e)
      echo "Built: $APP_BIN"
      ;;
    microcode_tests)
      echo "Built: $TEST_BIN"
      ;;
    *)
      echo "Built target: $target"
      ;;
  esac
done
