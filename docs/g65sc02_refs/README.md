# G65SC02 Grid References

This folder mirrors the high-value G65SC02 opcode/cycle grid artifacts used for cross-checking CPU behavior.

## Included Files

- `cmd_g65sc02_cycles_page9.html`
  - Readable CMD Page-9 style opcode matrix (bytes/cycles).
- `cmd_g65sc02_microcode_matrix.csv`
  - Full microcode-focused opcode matrix.
- `g65sc02_cycle_table_from_java.csv`
  - Full JVM cycle table export.
- `g65sc02_cmd_vs_java_cycles.md`
- `g65sc02_cmd_vs_java_cycles.csv`
  - CMD table-vs-JVM cycle comparison summaries.
- `g65sc02_vs_java_opcode_table.md`
- `g65sc02_vs_java_opcode_table.csv`
  - Opcode presence table (`G65SC02` vs current JVM table).

## Provenance

These files were copied from:
- `/Users/shane/Project/ever2e-jvm/docs/`

If upstream JVM docs are updated, refresh this folder by re-copying the files.

## Keyboard Repeat Timing Note

For Apple IIe keyboard auto-repeat timing discussion, see Jim Sather's
_Understanding the Apple IIe_ excerpt:

- https://www.dmcmillan.co.uk/blog/apple-iie-without-a-keyboard/Understanding_the_Apple_IIe.pdf

Important scope note:
- This is based on the original 1985 IIe-era analysis and is not a
  Platinum-specific timing source.

Additional supporting references:
- Apple Tech Info Library article on disabling IIe auto-repeat at keyboard
  encoder D14 pin 5 (confirms hardware ownership of repeat):
  - https://savagetaylor.com/TIL/TIL00268.pdf
- Apple II Technical Notes compilation (includes IIe board-revision notes;
  useful for Platinum-era change tracking):
  - https://mirrors.apple2.org.za/Apple%20II%20Documentation%20Project/Companies/Apple/Documentation/Apple%20II%20Technical%20Notes%201989-09.pdf
