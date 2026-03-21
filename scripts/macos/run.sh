#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/macos}"
OUTPUT_BIN="${OUTPUT_BIN:-$BUILD_DIR/ever2e}"
RELEASE_DIR="$ROOT_DIR/release"
REBUILD_ON_RUN="${REBUILD_ON_RUN:-1}"
DEFAULT_EMU_FILE="${DEFAULT_EMU_FILE:-Apple2eNoSlots.emu}"

if [[ "$REBUILD_ON_RUN" == "1" || ! -x "$OUTPUT_BIN" ]]; then
  BUILD_TARGETS="${BUILD_TARGETS:-ever2e}" "$ROOT_DIR/scripts/macos/build.sh"
fi

if [[ ! -d "$RELEASE_DIR" ]]; then
  echo "Error: release directory not found at $RELEASE_DIR" >&2
  exit 1
fi

EMU_ARG=""

if [[ "$#" -eq 0 || "$1" == --* ]]; then
  EMU_ARG="$DEFAULT_EMU_FILE"
else
  EMU_ARG="$1"
  shift
fi

# Convenience path resolution:
# - bare names like "Apple2eNoSlots.emu" resolve to release/<name>
# - relative paths that do not exist from repo root fallback to release/<path>
if [[ "$EMU_ARG" != /* ]]; then
  if [[ "$EMU_ARG" != *"/"* && "$EMU_ARG" != *"\\"* ]]; then
    EMU_ARG="$RELEASE_DIR/$EMU_ARG"
  elif [[ ! -f "$EMU_ARG" && -f "$RELEASE_DIR/$EMU_ARG" ]]; then
    EMU_ARG="$RELEASE_DIR/$EMU_ARG"
  fi
fi

if [[ ! -f "$EMU_ARG" ]]; then
  echo "Error: .emu config not found: $EMU_ARG" >&2
  exit 1
fi

exec "$OUTPUT_BIN" "$EMU_ARG" "$@"
