#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
RUN_SH="$ROOT_DIR/scripts/macos/run.sh"
EMU_FILE="${EMU_FILE:-$ROOT_DIR/release/Apple2eNoSlots.emu}"
QUIET_BUILD="${QUIET_BUILD:-1}"
ROM_FILE="${ROM_FILE:-}"
ROM_GENERATED="${ROM_GENERATED:-loop}"
REQUIRE_HALT_PC="${REQUIRE_HALT_PC:-}"
TMP_ROM_FILE=""

STEPS="${STEPS:-12}"

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
if [[ -z "$REQUIRE_HALT_PC" && "$ROM_GENERATED" == "loop" ]]; then
  REQUIRE_HALT_PC="0xC000"
fi
if [[ -n "$ROM_FILE" && ! -f "$ROM_FILE" ]]; then
  echo "Error: ROM file not found: $ROM_FILE" >&2
  exit 1
fi

ARGS=(
  --headless
  --steps "$STEPS"
  --print-cpu-state-at-exit
)

if [[ -n "$ROM_FILE" ]]; then
  ARGS+=(--rom-file "$ROM_FILE")
fi
if [[ -n "$REQUIRE_HALT_PC" ]]; then
  ARGS+=(--require-halt-pc "$REQUIRE_HALT_PC")
fi

env QUIET_BUILD="$QUIET_BUILD" "$RUN_SH" \
  "$EMU_FILE" \
  "${ARGS[@]}" \
  "$@"

echo "SMOKE PASS: steps=$STEPS final_pc=${REQUIRE_HALT_PC:-unchecked}"
