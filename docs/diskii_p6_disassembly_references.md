# Disk II P6 Disassembly/Provenance References (ever2e-cpp)

This document records where Disk II P6 ROM compatibility assumptions come from for the C++ side, and where to look when validating cycle-critical behavior.

## Scope

- 5.25" Disk II style controller ROM compatibility expectations
- P6 signature-byte preservation constraints
- cycle-sensitive path validation in local tooling/tests
- nibble/sector formatting assumptions used by conversion utilities

## Current C++-Side Implementation/Tooling Pointers

### DSK -> NIB Conversion

- `/Users/shane/Project/ever2e-cpp/tools/dsk_to_nib.cpp`

This file includes explicit comments and logic for:

- DOS 3.3 style 6-and-2 pre-nibble handling
- address/data prolog/epilog bytes
- sync cadence and per-track layout assumptions
- physical-sector ordering and payload emission

### Emulator Core (ROM/slot/IO behavior)

- `/Users/shane/Project/ever2e-cpp/src/memory128k.cpp`

Use this when confirming ROM selection paths and slot/IO mapping behavior that can affect Disk II ROM visibility.

## P6 Signature Constraints To Preserve

Compatibility bytes commonly used by ProDOS/Disk II detection logic:

- `offset $01 = $20`
- `offset $03 = $00`
- `offset $05 = $03`
- `offset $07 = $3C`
- `offset $FF = $00` (16-sector style)

If custom P6 refactors are done, keep these pinned unless intentionally testing nonstandard behavior.

## Where Cycle/Entry Validation Lives Today

Primary cycle/entry-point validation currently lives in the paired ROM workspace (`ever2e-rom`) and can be used as reference while iterating C++ behavior:

- `/Users/shane/Project/ever2e-rom/scripts/test_diskii_p6_rom.py`
- `/Users/shane/Project/ever2e-rom/tests/test_diskii_p6_entrypoints.py`

## External References Used

- Apple IIe Technical Reference Manual
- Jim Sather, *Understanding the Apple IIe*
- DOS 3.3 / RWTS-era disk-format notes (local project references)
- CiderPress II format notes (nibble/unadorned)

## Local-Only Source Policy Notes

Stock Disk II ROM byte images and disassembly sources that are copyright-sensitive should be kept local-only and not committed if licensing/distribution is unclear. Keep only metadata/fingerprints, compatibility signatures, and test expectations in committed docs/code.
