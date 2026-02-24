#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
RUN_SH="$ROOT_DIR/scripts/macos/run.sh"

PASTE_FILE="${PASTE_FILE:-$ROOT_DIR/release/opcode_smoke_loader_hgr_mem_16k.mon}"
STEPS="${STEPS:-80000000}"
HALT_EXECUTION="${HALT_EXECUTION:-0x33D2,0x33C0}"
REQUIRE_HALT_PC="${REQUIRE_HALT_PC:-0x33D2}"

if [[ ! -f "$PASTE_FILE" ]]; then
  echo "Error: smoke paste file not found: $PASTE_FILE" >&2
  exit 1
fi

exec "$RUN_SH" \
  --headless \
  --paste-file "$PASTE_FILE" \
  --steps "$STEPS" \
  --halt-execution "$HALT_EXECUTION" \
  --require-halt-pc "$REQUIRE_HALT_PC" \
  --print-cpu-state-at-exit \
  --print-text-at-exit \
  "$@"
