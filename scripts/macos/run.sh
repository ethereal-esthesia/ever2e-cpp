#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build/macos}"
OUTPUT_BIN="${OUTPUT_BIN:-$BUILD_DIR/ever2e}"
RELEASE_DIR="$ROOT_DIR/release"
REBUILD_ON_RUN="${REBUILD_ON_RUN:-1}"

if [[ "$REBUILD_ON_RUN" == "1" || ! -x "$OUTPUT_BIN" ]]; then
  "$ROOT_DIR/scripts/macos/build.sh"
fi

if [[ ! -d "$RELEASE_DIR" ]]; then
  echo "Error: release directory not found at $RELEASE_DIR" >&2
  exit 1
fi

cd "$RELEASE_DIR"
exec "$OUTPUT_BIN" "$@"
