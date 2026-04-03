# sw-yocto-ed MVP: COR24 C Text Editor

Build a minimal modal text editor for COR24 using tc24r compiler, cor24-run emulator, and reg-rs for testing. This is a dogfooding project that validates the full COR24 C toolchain.

## Step 1: AGENTS.md + Project Infrastructure
Create AGENTS.md with agentrail protocol adapted for C projects (tc24r build + reg-rs tests, no cargo/rustfmt). Create justfile for build/run/test. Remove gcc Makefile. Create .gitignore for build artifacts.

## Step 2: Platform Layer for COR24 UART
Adapt platform_stdio.c to use COR24 UART MMIO (0xFF0100/0xFF0101) via direct register access like tinyc demos. Use putchar/getchar pattern from tc24r stdio.h. The platform layer provides sye_platform_read_char() and sye_platform_write_string().

## Step 3: Build Smoke Test
Get the existing skeleton code compiling with tc24r and running on cor24-run. Verify initial render output via UART. Create first reg-rs test (yocto_ed prefix). Fix any tc24r compatibility issues (C subset limitations, 24-bit types, etc.).

## Step 4: Core Buffer Operations on COR24
Verify gap buffer operations work correctly on COR24: insert, delete, move cursor. Test via UART command sequences. Add reg-rs tests for buffer edge cases.

## Step 5: Edit Mode on COR24
Verify character insert, newline, backspace work via UART input. Test edit mode transitions to command mode via ESC. Add reg-rs tests.

## Step 6: Command Mode on COR24
Verify command parsing and execution via UART: left, right, del, backdel, newline, insert, save, new, quit, help. Test repeat counts. Add reg-rs tests.

## Step 7: Remaining Commands + Integration Test
Add missing commands from design doc (up, down, bol, eol, bob, eob, fwdw, backw, delline, goto). Create comprehensive integration reg-rs test that exercises full editor workflow: open, edit, navigate, insert, save, quit.

## Step 8: Polish + Final Validation
Run full reg-rs test suite. Verify all commands work end-to-end on emulator. Clean up code. Final commit with AGENTS.md complete.