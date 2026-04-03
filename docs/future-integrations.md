# sw-yocto-ed — Future Host Integration Notes

## Purpose

This document captures two host-side ideas that are intentionally out of MVP scope but should influence the shape of the command language now:

1. a Rust host-side UART bridge/client
2. an Emacs/Elisp remote editing mode

The current MVP should remain transport-safe and ASCII-oriented so that both future integrations can target the same command vocabulary.

---

## Guiding Principle

The COR24 editor should own the canonical editing model.

Host-side tools should be adapters that:
- capture richer local input
- translate it into stable remote commands
- optionally improve display and ergonomics
- avoid redefining core editor behavior

That keeps the COR24 system honest and prevents the host UI from becoming the real editor.

---

## Future Idea 1: Rust UART Bridge

### Goal

Create a host-side Rust program that acts as a smarter terminal bridge between a local machine and the COR24 UART.

### Why

A Rust bridge can do things that are awkward in the MVP editor itself:
- raw terminal handling on Linux/macOS
- decode local control keys reliably
- map local keys to transport-safe commands
- log traffic for debugging
- replay sessions for testing
- optionally provide better redisplay behavior

### Responsibilities

A future Rust bridge might:
- open a serial device
- place the host terminal into raw mode
- forward bytes to and from COR24
- translate local key sequences into textual editor commands
- optionally maintain a local command history
- support scripting and batch replay

### Suggested Non-Goals

The Rust bridge should not:
- become a full local editor
- own the canonical buffer state
- depend on screen-oriented tricks that the remote editor does not understand

### Good First Version

A small bridge could support:
- serial open/close
- raw mode enable/disable
- byte logging
- optional mapping such as:
  - local `Ctrl-U 8 Right` → remote `8 right\n`
  - local `Alt-f` → remote `fwdw\n`
  - local `Alt-b` → remote `backw\n`

### Later Possibilities

- session capture/replay
- integration tests against emulator instances
- remote protocol framing if plain text evolves later
- local macros that emit command sequences

---

## Future Idea 2: Emacs / Elisp Remote Editing Mode

### Goal

Create an Emacs mode that uses the COR24 editor as a remote editing target over UART, somewhat analogous in spirit to TRAMP, but much smaller and more specialized.

### Why

Emacs already solves many difficult local interaction problems:
- GUI and TTY input decoding
- keybinding customization
- command history
- major/minor mode composition
- macros
- scripting

An Elisp front-end can therefore provide a richer local user experience while still exercising the COR24 editor core.

### Conceptual Model

The Emacs side would act as:
- a transport adapter
- a command generator
- possibly a display helper

It should not assume full Emacs compatibility inside COR24.

### Possible Design Directions

#### Option A: Command-Only Front-End
Emacs sends textual commands over UART and displays remote output in a comint-like or special-purpose buffer.

Pros:
- simpler
- closest to MVP transport model
- easier to debug

Cons:
- limited UI richness

#### Option B: Structured Remote Editing Mode
Emacs maintains a local buffer view and sends editing commands to COR24, receiving state snapshots or line updates.

Pros:
- more ergonomic
- could feel more like a remote editor

Cons:
- more protocol complexity
- risk of Emacs side becoming the real source of truth

### Recommended Direction

Start with Option A later:
- special major mode
- serial transport process
- commands mapped to remote textual protocol
- remote output shown in a dedicated buffer

### Example Mappings

- `C-f` → send `right\n`
- `C-b` → send `left\n`
- `M-f` → send `fwdw\n`
- `M-b` → send `backw\n`
- `C-n` → send `down\n`
- `C-p` → send `up\n`
- `C-a` → send `bol\n`
- `C-e` → send `eol\n`

For insertions, the first version may simply send literal characters in edit mode and textual commands in command mode.

### Benefits

- lets you use familiar Emacs keys locally
- creates a practical dogfooding path
- helps refine remote command vocabulary
- gives a migration path toward a more programmable editor environment

### Risks

- protocol drift between Emacs front-end and COR24 implementation
- temptation to emulate too much Emacs behavior too early
- added debugging complexity across host/target boundary

---

## Implications for MVP Command Design

To preserve these future options, the MVP command set should be:
- textual
- regular
- stable
- easy to emit from scripts and host adapters

Recommended command form:

```text
[count] command [args...]
```

Examples:

```text
8 right
80 insert "*"
5 down
save
```

This keeps the COR24 editor transport-safe now while leaving room for richer input clients later.
