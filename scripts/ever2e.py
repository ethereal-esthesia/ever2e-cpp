#!/usr/bin/env python3
"""Portable build, run, and smoke-test helper for ever2e."""

from __future__ import annotations

import difflib
import filecmp
import os
import platform
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


ROOT_DIR = Path(__file__).resolve().parents[1]
PROFILE_DIR = ROOT_DIR / "profiles"
DEFAULT_PROFILE = "apple2e.emu"


def env(name: str, default: str = "") -> str:
    return os.environ.get(name, default)


def bool_env(name: str, default: bool) -> bool:
    raw = env(name)
    if raw == "":
        return default
    return raw not in ("0", "false", "False", "no", "NO")


def fail(message: str) -> int:
    print(f"Error: {message}", file=sys.stderr)
    return 1


def host_name() -> str:
    system = platform.system()
    if system == "Darwin":
        return "macos"
    if system == "Linux":
        return "linux"
    if system == "Windows":
        return "windows"
    return system.lower() or "host"


def default_preset() -> str:
    return env("PRESET", f"{host_name()}-release")


def build_dir() -> Path:
    return Path(env("BUILD_DIR", str(ROOT_DIR / "build" / host_name()))).expanduser()


def exe_suffix() -> str:
    return ".exe" if os.name == "nt" else ""


def executable_path(name: str, build_directory: Path | None = None) -> Path:
    explicit = env("OUTPUT_BIN")
    if explicit:
        return Path(explicit).expanduser()
    build_directory = build_directory or build_dir()
    exe_name = name + exe_suffix()
    candidates = [
        build_directory / exe_name,
        build_directory / "Release" / exe_name,
        build_directory / "RelWithDebInfo" / exe_name,
        build_directory / "Debug" / exe_name,
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate
    return candidates[0]


def run_process(
    args: list[str | Path],
    *,
    cwd: Path = ROOT_DIR,
    quiet: bool = False,
    check: bool = False,
) -> subprocess.CompletedProcess[str]:
    string_args = [str(arg) for arg in args]
    if quiet:
        proc = subprocess.run(
            string_args,
            cwd=str(cwd),
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        if proc.returncode != 0 and proc.stdout:
            print(proc.stdout, end="")
    else:
        proc = subprocess.run(string_args, cwd=str(cwd), text=True)
    if check and proc.returncode != 0:
        raise subprocess.CalledProcessError(proc.returncode, string_args)
    return proc


def ensure_cmake() -> int:
    if shutil.which("cmake") is None:
        return fail("cmake is required but was not found in PATH.")
    return 0


def parse_targets(default: str = "ever2e microcode_tests") -> list[str]:
    raw = env("BUILD_TARGETS", default)
    return [part for part in raw.split() if part]


def build_targets(targets: list[str] | None = None, *, quiet: bool | None = None) -> int:
    if ensure_cmake() != 0:
        return 1
    targets = targets or parse_targets()
    if not targets:
        return fail("BUILD_TARGETS resolved to an empty target list.")
    quiet = bool_env("QUIET_BUILD", False) if quiet is None else quiet
    preset = default_preset()
    try:
        run_process(["cmake", "--preset", preset], quiet=quiet, check=True)
        run_process(["cmake", "--build", "--preset", preset, "--target", *targets], quiet=quiet, check=True)
    except subprocess.CalledProcessError:
        return 1

    for target in targets:
        if target in ("ever2e", "microcode_tests"):
            print(f"Built: {executable_path(target)}", flush=True)
        else:
            print(f"Built target: {target}", flush=True)
    return 0


def command_build(args: list[str]) -> int:
    targets = args if args else None
    return build_targets(targets)


def command_clean(args: list[str]) -> int:
    if args:
        return fail("clean does not accept positional arguments.")
    directory = build_dir()
    shutil.rmtree(directory, ignore_errors=True)
    print(f"Removed: {directory}")
    return 0


def resolve_profile(raw: str) -> Path:
    raw_path = Path(raw).expanduser()
    profile_dir = Path(env("PROFILE_DIR", str(PROFILE_DIR))).expanduser()

    has_separator = "/" in raw or "\\" in raw
    if raw_path.is_absolute():
        return raw_path
    if not has_separator:
        return profile_dir / raw_path
    if raw_path.exists():
        return raw_path
    fallback = profile_dir / raw_path
    if fallback.exists():
        return fallback
    return raw_path


def split_profile_args(args: list[str]) -> tuple[Path, list[str]]:
    default_profile = env("DEFAULT_EMU_FILE", DEFAULT_PROFILE)
    if not args or args[0].startswith("-"):
        return resolve_profile(default_profile), args
    return resolve_profile(args[0]), args[1:]


def command_run(args: list[str]) -> int:
    profile, passthrough = split_profile_args(args)
    if not profile.exists():
        return fail(f".emu config not found: {profile}")

    binary = executable_path("ever2e")
    rebuild = bool_env("REBUILD_ON_RUN", True)
    if rebuild or not binary.exists():
        result = build_targets(["ever2e"], quiet=bool_env("QUIET_BUILD", False))
        if result != 0:
            return result
        binary = executable_path("ever2e")

    return run_process([binary, profile, *passthrough]).returncode


def write_loop_rom(path: Path) -> None:
    rom = bytearray(0x4000)
    rom[0:3] = bytes([0x4C, 0x00, 0xC0])
    for vector in (0x3FFA, 0x3FFC, 0x3FFE):
        rom[vector] = 0x00
        rom[vector + 1] = 0xC0
    path.write_bytes(rom)


def prepare_rom(rom_generated_default: str) -> tuple[str, tempfile.TemporaryDirectory[str] | None, int]:
    rom_file = env("ROM_FILE")
    rom_generated = env("ROM_GENERATED", rom_generated_default)
    if rom_file and rom_generated:
        return "", None, fail("set only one of ROM_FILE or ROM_GENERATED")
    if rom_generated:
        if rom_generated != "loop":
            return "", None, fail(f"unsupported ROM_GENERATED mode: {rom_generated} (expected: loop)")
        tmp = tempfile.TemporaryDirectory(prefix="ever2e-rom.")
        rom_path = Path(tmp.name) / "loop.rom"
        write_loop_rom(rom_path)
        return str(rom_path), tmp, 0
    if rom_file:
        expanded = Path(rom_file).expanduser()
        if not expanded.exists():
            return "", None, fail(f"ROM file not found: {rom_file}")
        return str(expanded), None, 0
    return "", None, 0


def run_emulator(profile: Path, emulator_args: list[str], passthrough: list[str]) -> int:
    return command_run([str(profile), *emulator_args, *passthrough])


def command_smoke(args: list[str]) -> int:
    profile = Path(env("EMU_FILE", str(ROOT_DIR / "profiles" / DEFAULT_PROFILE))).expanduser()
    if not profile.exists():
        return fail(f".emu config not found: {profile}")

    rom_file, tmp, result = prepare_rom("loop")
    if result != 0:
        return result
    try:
        steps = env("STEPS", "12")
        require_halt_pc = env("REQUIRE_HALT_PC")
        if not require_halt_pc and env("ROM_GENERATED", "loop") == "loop":
            require_halt_pc = "0xC000"

        emulator_args = ["--headless", "--steps", steps, "--print-cpu-state-at-exit"]
        if rom_file:
            emulator_args += ["--rom-file", rom_file]
        if require_halt_pc:
            emulator_args += ["--require-halt-pc", require_halt_pc]

        code = run_emulator(profile, emulator_args, args)
        if code == 0:
            print(f"SMOKE PASS: steps={steps} final_pc={require_halt_pc or 'unchecked'}")
        return code
    finally:
        if tmp is not None:
            tmp.cleanup()


def command_generated_tests(args: list[str]) -> int:
    profile = Path(env("EMU_FILE", str(ROOT_DIR / "profiles" / DEFAULT_PROFILE))).expanduser()
    paste_file = Path(
        env("PASTE_FILE", str(ROOT_DIR / "tests" / "fixtures" / "opcode_smoke_loader_hgr_mem_32k.mon"))
    ).expanduser()
    if not paste_file.exists():
        return fail(f"smoke paste file not found: {paste_file}")
    if not profile.exists():
        return fail(f".emu config not found: {profile}")

    rom_file, tmp, result = prepare_rom("")
    if result != 0:
        return result
    try:
        emulator_args = [
            "--headless",
            "--paste-file",
            str(paste_file),
            "--steps",
            env("STEPS", "200000000"),
            "--halt-execution",
            env("HALT_EXECUTION", "0x6A45,0x6A33"),
            "--require-halt-pc",
            env("REQUIRE_HALT_PC", "0x6A45"),
            "--print-cpu-state-at-exit",
            "--print-text-at-exit",
        ]
        if rom_file:
            emulator_args += ["--rom-file", rom_file]
        if env("TRACE_STEPS_FROM"):
            emulator_args += ["--trace-steps-from", env("TRACE_STEPS_FROM")]
        if env("TRACE_STEPS_COUNT"):
            emulator_args += ["--trace-steps-count", env("TRACE_STEPS_COUNT")]
        return run_emulator(profile, emulator_args, args)
    finally:
        if tmp is not None:
            tmp.cleanup()


def command_microcode_tests(args: list[str]) -> int:
    if args:
        return fail("microcode-tests does not accept positional arguments.")
    binary = executable_path("microcode_tests")
    rebuild = bool_env("REBUILD_ON_RUN", True)
    if rebuild or not binary.exists():
        result = build_targets(["microcode_tests"], quiet=bool_env("QUIET_BUILD", False))
        if result != 0:
            return result

    if shutil.which("ctest"):
        return run_process(["ctest", "--preset", default_preset(), "-R", "^microcode_tests$"]).returncode
    return run_process([executable_path("microcode_tests")]).returncode


def command_main_tests(args: list[str]) -> int:
    if args:
        return fail("main-tests does not accept positional arguments.")
    result = command_microcode_tests([])
    if result != 0:
        return result
    return command_generated_tests([])


def command_trace_baseline(args: list[str]) -> int:
    profile = Path(env("EMU_FILE", str(ROOT_DIR / "profiles" / DEFAULT_PROFILE))).expanduser()
    if not profile.exists():
        return fail(f".emu config not found: {profile}")

    baseline_file = Path(
        env("BASELINE_FILE", str(ROOT_DIR / "tests" / "baselines" / "cpp_loop_trace_verbose.csv"))
    ).expanduser()
    actual_file = Path(env("ACTUAL_FILE", str(build_dir() / "trace_loop_actual.csv"))).expanduser()
    baseline_file.parent.mkdir(parents=True, exist_ok=True)
    actual_file.parent.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory(prefix="ever2e-trace-rom.") as tmp:
        rom_path = Path(tmp) / "loop.rom"
        write_loop_rom(rom_path)
        code = run_emulator(
            profile,
            [
                "--headless",
                "--steps",
                env("STEPS", "12"),
                "--rom-file",
                str(rom_path),
                "--require-halt-pc",
                "0xC000",
                "--trace-file",
                str(actual_file),
                "--trace-verbose",
            ],
            args,
        )
        if code != 0:
            return code

    if env("UPDATE_BASELINE", "0") == "1" or not baseline_file.exists():
        shutil.copyfile(actual_file, baseline_file)
        print(f"TRACE BASELINE UPDATED: {baseline_file}")
        return 0

    if filecmp.cmp(baseline_file, actual_file, shallow=False):
        print(f"TRACE PARITY PASS: {actual_file} matches {baseline_file}")
        return 0

    print(f"TRACE PARITY FAIL: {actual_file} differs from {baseline_file}", file=sys.stderr)
    baseline = baseline_file.read_text(errors="replace").splitlines(keepends=True)
    actual = actual_file.read_text(errors="replace").splitlines(keepends=True)
    sys.stderr.writelines(difflib.unified_diff(baseline, actual, fromfile=str(baseline_file), tofile=str(actual_file)))
    return 1


def write_p6_boot_fixture(out: Path) -> Path:
    track_total = 35
    track_bytes = 416 * 16
    boot_track = 34
    payload_addr = 0x0800
    payload_loop = payload_addr + 0x0A
    boot_mark_addr = 0x0400

    loader = bytes(
        [
            0xA2, 0x20, 0xA0, 0x00, 0xA2, 0x03, 0x86, 0x3C, 0xA2, 0x60, 0xBD, 0x89, 0xC0, 0xBD, 0x8E, 0xC0,
            0x20, 0x49, 0xC6, 0xC9, 0xE2, 0xD0, 0xF9, 0x20, 0x49, 0xC6, 0xC9, 0xB0, 0xD0, 0xF2, 0x20, 0x49,
            0xC6, 0xC9, 0xB1, 0xD0, 0xEB, 0x20, 0x49, 0xC6, 0xC9, 0xB2, 0xD0, 0xE4, 0xA0, 0x00, 0x20, 0x49,
            0xC6, 0x29, 0x0F, 0x0A, 0x0A, 0x0A, 0x0A, 0x85, 0x3D, 0x20, 0x49, 0xC6, 0x29, 0x0F, 0x05, 0x3D,
            0x99, 0x00, 0x08, 0xC8, 0xD0, 0xE8, 0x4C, 0x00, 0x08, 0xBD, 0x8C, 0xC0, 0x10, 0xFB, 0x60,
        ]
    )

    slot_rom = bytearray([0xEA] * 0x100)
    slot_rom[: len(loader)] = loader
    slot_rom[0xFF] = 0x00

    payload = bytearray([0xEA] * 0x100)
    payload[:13] = bytes(
        [
            0xA9,
            0x42,
            0x8D,
            boot_mark_addr & 0xFF,
            (boot_mark_addr >> 8) & 0xFF,
            0xA9,
            0xC8,
            0x8D,
            (boot_mark_addr + 1) & 0xFF,
            ((boot_mark_addr + 1) >> 8) & 0xFF,
            0x4C,
            payload_loop & 0xFF,
            (payload_loop >> 8) & 0xFF,
        ]
    )

    encoded = bytearray()
    for value in payload:
        encoded.append(0xA0 | (value >> 4))
        encoded.append(0xA0 | (value & 0x0F))

    nib = bytearray([0xFF] * (track_total * track_bytes))
    pos = boot_track * track_bytes + 32
    nib[pos : pos + 4] = bytes([0xE2, 0xB0, 0xB1, 0xB2])
    pos += 4
    nib[pos : pos + len(encoded)] = encoded

    full_rom = bytearray([0xEA] * 0x4000)
    full_rom[0x600:0x700] = slot_rom
    for vector in (0x3FFA, 0x3FFC, 0x3FFE):
        full_rom[vector] = 0x00
        full_rom[vector + 1] = 0xC6

    (out / "DISKII_P6_BOOT_TEST.rom").write_bytes(slot_rom)
    (out / "DISKII_P6_BOOT_TEST.nib").write_bytes(nib)
    (out / "P6_BOOT_16K.rom").write_bytes(full_rom)
    profile = out / "custom-p6-boot.emu"
    profile.write_text(
        """machine.layout=APPLE_IIE
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
machine.layout.slot.6.rom.file=DISKII_P6_BOOT_TEST.rom
machine.layout.slot.6.drive.1.file=DISKII_P6_BOOT_TEST.nib
machine.layout.slot.6.drive.2.file=
machine.layout.slot.7=
""",
        encoding="utf-8",
    )
    return profile


def command_p6_boot_smoke(args: list[str]) -> int:
    with tempfile.TemporaryDirectory(prefix="ever2e-p6-boot-smoke.") as tmp:
        profile = write_p6_boot_fixture(Path(tmp))
        code = run_emulator(
            profile,
            [
                "--headless",
                "--steps",
                env("STEPS", "300000"),
                "--halt-execution",
                "0x080A",
                "--require-halt-pc",
                "0x080A",
                "--print-cpu-state-at-exit",
            ],
            args,
        )
        if code != 0:
            return code
    print("P6 BOOT SMOKE PASS: generated clean custom-stream slot ROM + generated custom boot NIB")
    return 0


COMMANDS = {
    "build": command_build,
    "clean": command_clean,
    "run": command_run,
    "smoke": command_smoke,
    "generated-tests": command_generated_tests,
    "microcode-tests": command_microcode_tests,
    "main-tests": command_main_tests,
    "trace-baseline": command_trace_baseline,
    "p6-boot-smoke": command_p6_boot_smoke,
}


def print_help() -> None:
    commands = "\n".join(f"  {name}" for name in sorted(COMMANDS))
    print(
        f"""Usage: {Path(sys.argv[0]).name} <command> [args...]

Commands:
{commands}

Common environment overrides:
  PRESET, BUILD_DIR, BUILD_TARGETS, QUIET_BUILD, REBUILD_ON_RUN
  EMU_FILE, ROM_FILE, ROM_GENERATED, STEPS
"""
    )


def main(argv: list[str]) -> int:
    if not argv or argv[0] in ("-h", "--help", "help"):
        print_help()
        return 0
    command = argv[0]
    fn = COMMANDS.get(command)
    if fn is None:
        print_help()
        return fail(f"unknown command: {command}")
    return fn(argv[1:])


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
