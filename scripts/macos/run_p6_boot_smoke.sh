#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
RUN_SH="$ROOT_DIR/scripts/macos/run.sh"
QUIET_BUILD="${QUIET_BUILD:-1}"
STEPS="${STEPS:-300000}"
TMP_DIR="$(mktemp -d "${TMPDIR:-/tmp}/ever2e-p6-boot-smoke.XXXXXX")"

cleanup() {
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

python3 - "$TMP_DIR" <<'PY'
from pathlib import Path
import sys

out = Path(sys.argv[1])
track_total = 35
track_bytes = 416 * 16
boot_track = 34
payload_addr = 0x0800
payload_loop = payload_addr + 0x0A
boot_mark_addr = 0x0400

loader = bytes([
    0xA2,0x20,0xA0,0x00,0xA2,0x03,0x86,0x3C,0xA2,0x60,0xBD,0x89,0xC0,0xBD,0x8E,0xC0,
    0x20,0x49,0xC6,0xC9,0xE2,0xD0,0xF9,0x20,0x49,0xC6,0xC9,0xB0,0xD0,0xF2,0x20,0x49,
    0xC6,0xC9,0xB1,0xD0,0xEB,0x20,0x49,0xC6,0xC9,0xB2,0xD0,0xE4,0xA0,0x00,0x20,0x49,
    0xC6,0x29,0x0F,0x0A,0x0A,0x0A,0x0A,0x85,0x3D,0x20,0x49,0xC6,0x29,0x0F,0x05,0x3D,
    0x99,0x00,0x08,0xC8,0xD0,0xE8,0x4C,0x00,0x08,0xBD,0x8C,0xC0,0x10,0xFB,0x60
])

slot_rom = bytearray([0xEA] * 0x100)
slot_rom[:len(loader)] = loader
slot_rom[0xFF] = 0x00

payload = bytearray([0xEA] * 0x100)
payload[:13] = bytes([
    0xA9,0x42,
    0x8D,boot_mark_addr & 0xFF,(boot_mark_addr >> 8) & 0xFF,
    0xA9,0xC8,
    0x8D,(boot_mark_addr + 1) & 0xFF,((boot_mark_addr + 1) >> 8) & 0xFF,
    0x4C,payload_loop & 0xFF,(payload_loop >> 8) & 0xFF,
])

encoded = bytearray()
for value in payload:
    encoded.append(0xA0 | (value >> 4))
    encoded.append(0xA0 | (value & 0x0F))

nib = bytearray([0xFF] * (track_total * track_bytes))
pos = boot_track * track_bytes + 32
nib[pos:pos + 4] = bytes([0xE2, 0xB0, 0xB1, 0xB2])
pos += 4
nib[pos:pos + len(encoded)] = encoded

full_rom = bytearray([0xEA] * 0x4000)
full_rom[0x600:0x700] = slot_rom
for vector in (0x3FFA, 0x3FFC, 0x3FFE):
    full_rom[vector] = 0x00
    full_rom[vector + 1] = 0xC6

(out / "DISKII_P6_CUSTOM.rom").write_bytes(slot_rom)
(out / "DISKII_P6_BOOT_TEST.nib").write_bytes(nib)
(out / "P6_BOOT_16K.rom").write_bytes(full_rom)
(out / "custom-p6-boot.emu").write_text("""machine.layout=APPLE_IIE
binary.file=P6_BOOT_16K.rom
address.start=0xC000
machine.cpu.mult=1
machine.display.mono=off
machine.layout.slot.1=
machine.layout.slot.2=
machine.layout.slot.3=
machine.layout.slot.4=
machine.layout.slot.5=
machine.layout.slot.6=drive.floppy525.Floppy525Controller
machine.layout.slot.6.rom.file=DISKII_P6_CUSTOM.rom
machine.layout.slot.6.drive.1.file=DISKII_P6_BOOT_TEST.nib
machine.layout.slot.6.drive.2.file=
machine.layout.slot.7=
""", encoding="utf-8")
PY

env QUIET_BUILD="$QUIET_BUILD" "$RUN_SH" \
  "$TMP_DIR/custom-p6-boot.emu" \
  --headless \
  --steps "$STEPS" \
  --halt-execution 0x080A \
  --require-halt-pc 0x080A \
  --print-cpu-state-at-exit \
  "$@"

echo "P6 BOOT SMOKE PASS: generated clean slot ROM + generated custom boot NIB"
