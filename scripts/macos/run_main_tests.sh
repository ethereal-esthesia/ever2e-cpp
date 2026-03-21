#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

"$ROOT_DIR/scripts/macos/run_microcode_tests.sh"
"$ROOT_DIR/scripts/macos/run_generated_tests.sh"
