#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
RUN_SH="$ROOT_DIR/scripts/macos/run.sh"
EMU_FILE="${EMU_FILE:-$ROOT_DIR/release/Apple2eNoSlots.emu}"
QUIET_BUILD="${QUIET_BUILD:-1}"
ROM_FILE="${ROM_FILE:-}"
ROM_GENERATED="${ROM_GENERATED:-}"
TMP_ROM_FILE=""

PASTE_FILE="${PASTE_FILE:-$ROOT_DIR/release/opcode_smoke_loader_hgr_mem_32k.mon}"
STEPS="${STEPS:-200000000}"
HALT_EXECUTION="${HALT_EXECUTION:-0x6A45,0x6A33}"
REQUIRE_HALT_PC="${REQUIRE_HALT_PC:-0x6A45}"
TRACE_STEPS_FROM="${TRACE_STEPS_FROM:-}"
TRACE_STEPS_COUNT="${TRACE_STEPS_COUNT:-}"

cleanup() {
  if [[ -n "$TMP_ROM_FILE" && -f "$TMP_ROM_FILE" ]]; then
    rm -f "$TMP_ROM_FILE"
  fi
}
trap cleanup EXIT

if [[ ! -f "$PASTE_FILE" ]]; then
  echo "Error: smoke paste file not found: $PASTE_FILE" >&2
  exit 1
fi
if [[ ! -f "$EMU_FILE" ]]; then
  echo "Error: .emu config not found: $EMU_FILE" >&2
  exit 1
fi
if [[ -n "$ROM_FILE" && -n "$ROM_GENERATED" ]]; then
  echo "Error: set only one of ROM_FILE or ROM_GENERATED" >&2
  exit 1
fi
if [[ -n "$ROM_GENERATED" ]]; then
  if [[ "$ROM_GENERATED" != "loop" ]]; then
    echo "Error: unsupported ROM_GENERATED mode: $ROM_GENERATED (expected: loop)" >&2
    exit 1
  fi
  TMP_ROM_FILE="$(mktemp "${TMPDIR:-/tmp}/ever2e-rom.XXXXXX")"
  dd if=/dev/zero of="$TMP_ROM_FILE" bs=16384 count=1 status=none
  # $C000: JMP $C000 (infinite loop)
  printf '\x4C\x00\xC0' | dd of="$TMP_ROM_FILE" bs=1 seek=0 conv=notrunc status=none
  # Set NMI/RESET/IRQ vectors at FFFA/FFFC/FFFE to $C000.
  printf '\x00\xC0\x00\xC0\x00\xC0' | dd of="$TMP_ROM_FILE" bs=1 seek=$((0x3FFA)) conv=notrunc status=none
  ROM_FILE="$TMP_ROM_FILE"
fi
if [[ -n "$ROM_FILE" && ! -f "$ROM_FILE" ]]; then
  echo "Error: ROM file not found: $ROM_FILE" >&2
  exit 1
fi

ARGS=(
  --headless
  --paste-file "$PASTE_FILE"
  --steps "$STEPS"
  --halt-execution "$HALT_EXECUTION"
  --require-halt-pc "$REQUIRE_HALT_PC"
  --print-cpu-state-at-exit
  --print-text-at-exit
)

if [[ -n "$ROM_FILE" ]]; then
  ARGS+=(--rom-file "$ROM_FILE")
fi

if [[ -n "$TRACE_STEPS_FROM" ]]; then
  ARGS+=(--trace-steps-from "$TRACE_STEPS_FROM")
fi
if [[ -n "$TRACE_STEPS_COUNT" ]]; then
  ARGS+=(--trace-steps-count "$TRACE_STEPS_COUNT")
fi

exec env QUIET_BUILD="$QUIET_BUILD" "$RUN_SH" \
  "$EMU_FILE" \
  "${ARGS[@]}" \
  "$@"
