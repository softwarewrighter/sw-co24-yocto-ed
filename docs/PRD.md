# sw-yocto-ed — Product Requirements Document (PRD)

## Overview

sw-yocto-ed is a minimal modal text editor designed for the COR24 ISA environment. It operates over UART using a transport-safe ASCII command interface and a simple 3-line display model.

The editor is intended as a bootstrap system that will evolve toward a programmable, Lisp-driven editor.

---

## Goals

### Primary Goals
- Provide a usable text editor over UART with minimal dependencies
- Support basic text navigation, insertion, deletion, and saving
- Use a transport-safe command interface (ASCII only)
- Establish a foundation for future Macro Lisp integration

### Secondary Goals
- Enable scripting and automation via textual commands
- Provide a clean path to host-side integrations (Rust client, Emacs mode)

---

## Non-Goals (MVP)
- Full Emacs compatibility
- Multiple buffers
- Regions/mark system
- Undo/redo
- Syntax highlighting
- Full-screen terminal control (VT100, curses)

---

## User Interaction Model

### Modes

#### Edit Mode
- Default mode
- Printable characters insert at point
- Enter inserts newline
- Backspace deletes previous character
- Escape switches to Command Mode

#### Command Mode
- Accepts ASCII commands on input line
- Enter executes command
- Returns to Edit Mode after execution
- `C-]` cancels and returns to Edit Mode

---

## Display Model (3-line UI)
8 right
80 insert "*"
10 newline


Semantics:
- Prefix applies to command execution count
- Applies to motion, deletion, and insert commands
- Default count = 1

Future:
- optional Ctrl-U prefix for interactive use

---

## Data Model

- Single buffer
- Gap buffer representation
- Fixed memory regions
- Dirty flag
- Cursor (point)

---

## Success Criteria

- User can edit, navigate, and save text via UART
- Commands are consistent and predictable
- System runs within COR24 constraints
- Codebase supports future extension (Lisp, host UI)

---

## Future Directions

- Macro Lisp command binding
- Host-side Rust terminal bridge
- Emacs remote editing mode
- Regions and kill/yank
- Multiple buffers

