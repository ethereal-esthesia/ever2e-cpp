#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
RUN_SH="$ROOT_DIR/scripts/macos/run.sh"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/macos}"
EMU_FILE="${EMU_FILE:-$ROOT_DIR/release/Apple2eNoSlots.emu}"
QUIET_BUILD="${QUIET_BUILD:-1}"
STEPS="${STEPS:-12}"
UPDATE_BASELINE="${UPDATE_BASELINE:-0}"
BASELINE_FILE="${BASELINE_FILE:-$ROOT_DIR/tests/baselines/cpp_loop_trace_verbose.csv}"
ACTUAL_FILE="${ACTUAL_FILE:-$BUILD_DIR/trace_loop_actual.csv}"
TMP_ROM_FILE=""

cleanup() {
  if [[ -n "$TMP_ROM_FILE" && -f "$TMP_ROM_FILE" ]]; then
    rm -f "$TMP_ROM_FILE"
  fi
}
trap cleanup EXIT

if [[ ! -f "$EMU_FILE" ]]; then
  echo "Error: .emu config not found: $EMU_FILE" >&2
  exit 1
fi

mkdir -p "$(dirname "$BASELINE_FILE")"
mkdir -p "$(dirname "$ACTUAL_FILE")"

TMP_ROM_FILE="$(mktemp "${TMPDIR:-/tmp}/ever2e-trace-rom.XXXXXX")"
dd if=/dev/zero of="$TMP_ROM_FILE" bs=16384 count=1 status=none
# $C000: JMP $C000 (infinite loop)
printf '\x4C\x00\xC0' | dd of="$TMP_ROM_FILE" bs=1 seek=0 conv=notrunc status=none
# Set NMI/RESET/IRQ vectors at FFFA/FFFC/FFFE to $C000.
printf '\x00\xC0\x00\xC0\x00\xC0' | dd of="$TMP_ROM_FILE" bs=1 seek=$((0x3FFA)) conv=notrunc status=none

env QUIET_BUILD="$QUIET_BUILD" "$RUN_SH" \
  "$EMU_FILE" \
  --headless \
  --steps "$STEPS" \
  --rom-file "$TMP_ROM_FILE" \
  --require-halt-pc 0xC000 \
  --trace-file "$ACTUAL_FILE" \
  --trace-verbose \
  "$@"

if [[ "$UPDATE_BASELINE" == "1" || ! -f "$BASELINE_FILE" ]]; then
  cp "$ACTUAL_FILE" "$BASELINE_FILE"
  echo "TRACE BASELINE UPDATED: $BASELINE_FILE"
  exit 0
fi

if cmp -s "$BASELINE_FILE" "$ACTUAL_FILE"; then
  echo "TRACE PARITY PASS: $ACTUAL_FILE matches $BASELINE_FILE"
  exit 0
fi

echo "TRACE PARITY FAIL: $ACTUAL_FILE differs from $BASELINE_FILE" >&2
diff -u "$BASELINE_FILE" "$ACTUAL_FILE" || true
exit 1
