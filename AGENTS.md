# AGENTS.md

This file provides guidance to AI coding agents (opencode, Claude Code, Gemini CLI, etc.) when working with code in this repository.

## Project: sw-co24-yocto-ed -- Minimal Modal Text Editor for COR24

A dogfooding project: a small modal text editor (gap buffer, 3-line display, command mode)
written in C and built entirely with the COR24 toolchain (tc24r compiler, cor24-run emulator).
This validates the full tc24r C compiler, assembler, and emulator stack.

## CRITICAL: AgentRail Session Protocol (MUST follow exactly)

Every agent session follows this 6-step loop. Do NOT skip or reorder steps.

### 1. START (do this FIRST, before anything else)
```bash
agentrail next
```
Read the output carefully. It contains your current step, prompt, plan context, and any relevant skills/trajectories.

### 2. BEGIN (immediately after reading the next output)
```bash
agentrail begin
```

### 3. WORK (do what the step prompt says)
Do NOT ask "want me to proceed?" or "shall I start?". The step prompt IS your instruction. Execute it directly.

### 4. PRE-COMMIT QUALITY GATE (MANDATORY -- every step, no exceptions)
Every completed saga step must be high quality, documented, and pushed to GitHub.
If anything fails, fix the underlying problem -- NEVER suppress, allow, or work around a check.

#### Phase A: Build and test
1. `just build` -- Compile with tc24r. Fix any compilation errors.
2. `reg-rs run -p yocto_ed` -- All tests pass. Fix any failures.
3. Review staged files with `git status` -- No build artifacts, no unintended files.
4. Verify documentation is up-to-date -- if code changed affected behavior, update docs.
5. **Commit** the working, tested code.

#### Phase B: Push
6. `git push` -- Every completed step must be pushed to GitHub.

### 5. COMPLETE (LAST thing, after committing and pushing)
```bash
agentrail complete --summary "what you accomplished" \
  --reward 1 \
  --actions "tools and approach used"
```
- If the step failed: `--reward -1 --failure-mode "what went wrong"`
- If the saga is finished: add `--done`

### 6. STOP (after complete, DO NOT continue working)
Do NOT make further code changes after running `agentrail complete`.
Any changes after complete are untracked and invisible to the next session.
Future work belongs in the NEXT step, not this one.

## Key Rules

- **Do NOT skip steps** -- the next session depends on accurate tracking
- **Do NOT ask for permission** -- the step prompt is the instruction
- **Do NOT continue working** after `agentrail complete`
- **Commit before complete** -- always commit first, then record completion
- **NO gcc** -- this project uses only tc24r as the C compiler
- **NO host binary** -- we build COR24 assembly (.s) and run on the emulator
- **NO Python** -- do not use Python for anything in this project
- **ISA register naming** -- COR24 registers are named, not numbered. Use
  fp, sp, z, ir, iv (not r3, r4, r5, r6, r7).

## Useful Commands

```bash
agentrail status          # Current saga state
agentrail history         # All completed steps
agentrail plan            # View the plan
agentrail next            # Current step + context

just build                # Compile with tc24r
just run                  # Run on emulator (interactive terminal mode)
just test                 # Run reg-rs test suite
just clean                # Remove build artifacts
```

## Toolchain

| Tool | Purpose |
|------|---------|
| `tc24r` | C cross-compiler: C source -> COR24 assembly (.s) |
| `tc24r-pp` | C preprocessor |
| `cor24-run` | COR24 assembler + emulator |
| `reg-rs` | Regression test runner (golden output baselines) |

### Build Pipeline

```
src/*.c  --tc24r-->  build/swye.s  --cor24-run-->  emulate on COR24
```

### UART I/O

The editor communicates via COR24 UART registers:
- `0xFF0100` -- UART data (write to TX, read to RX)
- `0xFF0101` -- UART status (bit 7 = TX busy, bit 0 = RX ready)

The platform layer (`src/platform_uart.c`) provides:
- `sye_platform_read_char()` -- reads one character from UART RX
- `sye_platform_write_string()` -- writes string to UART TX
- `sye_platform_write_char()` -- writes single character to UART TX

### Testing with reg-rs

Tests use the `yocto_ed` prefix. Create tests with:
```bash
PP="grep -A 100 '^UART output:' || true"
reg-rs create -t yocto_ed_TESTNAME -P "$PP" --timeout 30 \
  -c "cor24-run --run build/swye.s -u 'INPUT' --speed 0 -n 5000000 2>&1" \
  --desc "description"
```

Run tests: `reg-rs run -p yocto_ed`
Rebase after intentional changes: `reg-rs rebase -p yocto_ed`

## Related Projects

All COR24 repos live under `~/github/sw-embed/` as siblings.

### Toolchain (this project validates these)
- `sw-cor24-x-tinyc` -- tc24r C cross-compiler (Rust)
- `sw-cor24-emulator` -- COR24 assembler and emulator (Rust)

### Reference C projects (pattern for UART I/O, build, testing)
- `sw-cor24-macrolisp` -- Tiny Macro Lisp (C, tc24r + justfile)
- `sw-cor24-plsw` -- PL/SW compiler (C, tc24r + justfile)

### Other COR24 languages
- `sw-cor24-forth` -- Forth interpreter
- `sw-cor24-apl` -- APL interpreter
- `sw-cor24-basic` -- BASIC interpreter
- `sw-cor24-pascal` -- Pascal compiler + runtime

## Architecture

- **Buffer** (`buffer.c`) -- Gap buffer: insert, delete, move, row/col tracking
- **Editor** (`editor.c`) -- Mode management, edit/command key dispatch, render loop
- **Command** (`command.c`) -- Parse `[count] command [args]`, execute with repeat
- **Render** (`render.c`) -- 3-line display: TEXT, MODE, CMD lines
- **Platform** (`platform_uart.c`) -- COR24 UART MMIO layer

## Design Constraints

- COR24 is 24-bit: int is 24-bit, pointers are 24-bit, address space is 16 MB
- tc24r supports a C subset: no long, no float, no struct copies, limited printf
- Memory layout: code at 0x000000, buffer at 0x010000, heap at 0x020000, stack in EBR
- Work buffer is a static 4096-byte array in BSS
- Single buffer only (no multiple files in MVP)
