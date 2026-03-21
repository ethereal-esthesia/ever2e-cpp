#!/usr/bin/env python3
"""Generate line-by-line floating-bus oracle values from ever2e-py video-probe predictor."""

from __future__ import annotations

import argparse
import sys


def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser()
    p.add_argument("--py-root", required=True, help="Path to ever2e-py repo root")
    p.add_argument("--cycles", type=int, default=1024)
    p.add_argument("--text", type=int, choices=(0, 1), default=0)
    p.add_argument("--mixed", type=int, choices=(0, 1), default=0)
    p.add_argument("--hires", type=int, choices=(0, 1), default=1)
    p.add_argument("--page2", type=int, choices=(0, 1), default=0)
    p.add_argument("--store80", type=int, choices=(0, 1), default=0)
    return p.parse_args()


def main() -> int:
    args = parse_args()
    sys.path.insert(0, args.py_root)

    try:
        from ever2e_py.apple2e_memory import Apple2eMemory
    except Exception as exc:  # pragma: no cover
        print(f"import-error: {exc}", file=sys.stderr)
        return 2

    mem = Apple2eMemory()
    mem.set_open_bus_source("video_probe")

    # Deterministic RAM pattern to make value comparisons stable.
    for i in range(0x10000):
        mem.main[i] = i & 0xFF

    mem.switch_text = bool(args.text)
    mem.switch_mixed = bool(args.mixed)
    mem.switch_hires = bool(args.hires)
    mem.switch_page2 = bool(args.page2)
    mem.switch80store = bool(args.store80)

    for t in range(max(0, args.cycles)):
        addr, val = mem.predict_open_bus_for_total_cycles(t)
        print(f"{t:05d},0x{addr:04X},0x{val:02X}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
