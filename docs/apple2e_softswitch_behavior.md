# Apple IIe Soft-Switch Behavior (ever2e-cpp)

This document pins the soft-switch behavior implemented in `ever2e-cpp`, with direct pointers to the code that currently defines read/write side effects.

## Primary Implementation Files

- `/Users/shane/Project/ever2e-cpp/include/memory128k.h`
- `/Users/shane/Project/ever2e-cpp/src/memory128k.cpp`
- `/Users/shane/Project/ever2e-cpp/src/mon560x192.cpp`
- `/Users/shane/Project/ever2e-cpp/src/keyboard2e.cpp`

## Core Flag Definitions

Switch flags are defined in:

- `/Users/shane/Project/ever2e-cpp/include/memory128k.h`

Notable flags used across display/memory routing:

- `_80STORE`, `_PAGE2`, `_HIRES`
- `_RAMRD`, `_RAMWRT`, `_80COL`
- `_AN3`
- `_INTCXROM`, `_SLOTC3ROM`, `_INTC8ROM`
- `_BANK1`, `_HRAMRD`, `_HRAMWRT`, `_PREWRITE`

## I/O Read/Write Handlers

Main soft-switch dispatch:

- `_readIo(...)` in `/Users/shane/Project/ever2e-cpp/src/memory128k.cpp`
- `_writeIo(...)` in `/Users/shane/Project/ever2e-cpp/src/memory128k.cpp`

These handlers implement C0xx side effects (including keyboard strobe/speaker/display and language-card style switching).

## Behavior Notes By Area

### Keyboard and Strobe

- `C000` returns keyboard data/strobe state.
- `C010` clears strobe handling path.
- `keyboard2e.cpp` documents the queue semantics: strobe clear follows KEYBOARD read/consume behavior.

Pointers:

- `/Users/shane/Project/ever2e-cpp/src/keyboard2e.cpp`
- `/Users/shane/Project/ever2e-cpp/include/keyboard2e.h`

### Display/Graphics Soft Switches

Display-related toggles are handled in `memory128k.cpp` and consumed by renderer selection in `mon560x192.cpp`.

Common addresses represented in implementation:

- `C050/C051` graphics/text
- `C052/C053` mixed/full
- `C054/C055` page1/page2
- `C056/C057` lores/hires
- `C05E/C05F` AN3 off/on

Display-mode interpretation path:

- `/Users/shane/Project/ever2e-cpp/src/mon560x192.cpp`

### Main/Aux Memory Routing

`_80STORE`, `_RAMRD`, `_RAMWRT`, `_PAGE2`, and `_HIRES` interaction for `0x400-0x7FF` and `0x2000-0x3FFF` is implemented in memory read/write routing helpers.

Pointers:

- `/Users/shane/Project/ever2e-cpp/src/memory128k.cpp` (comments near read/write mapping around 80STORE/HIRES checks)

### Slot/Internal ROM Selection

`_INTCXROM`, `_SLOTC3ROM`, and `_INTC8ROM` selection behavior is implemented in slot read logic and C3xx/C8xx handling.

Pointers:

- `/Users/shane/Project/ever2e-cpp/src/memory128k.cpp` (`_readSlot(...)` and related C3xx/C8xx logic)

### Language Card Related Switching

Language card bank/read/write select behavior is represented by `_BANK1`, `_HRAMRD`, `_HRAMWRT`, `_PREWRITE` and C08x handling.

Pointers:

- `/Users/shane/Project/ever2e-cpp/src/memory128k.cpp` (C080-C08F handling)

## External Hardware References Used

- Apple IIe Technical Reference Manual
- Jim Sather, *Understanding the Apple IIe*

These are the intended hardware-behavior references. Repo tests remain the practical regression authority for current emulator behavior.
