# sw-yocto-ed — Detailed Design

## Input Handling

### Edit Mode

| Input        | Action                  |
|-------------|--------------------------|
| printable   | insert char              |
| Enter       | insert newline           |
| Backspace   | delete previous char     |
| Escape      | switch to command mode   |

---

### Command Mode

| Input        | Action                      |
|-------------|-----------------------------|
| text        | build command string        |
| Backspace   | edit command                |
| Enter       | execute command             |
| C-]         | cancel, return to edit mode |

---

## Command Grammar
[count] command [arguments...]


Examples:
right
8 right
insert foo
80 insert "*"
goto 42


---

## Command Table

| Command   | Description |
|----------|------------|
| left/right/up/down | movement |
| bol/eol | line boundaries |
| bob/eob | buffer boundaries |
| fwdw/backw | word movement |
| del/backdel | char delete |
| delline | delete line |
| newline | insert newline |
| insert TEXT | insert string |
| save | save buffer |
| new | clear buffer |
| quit | exit |
| goto N | jump to line |
| help | show help |

---

## Repeat Semantics

- Default count = 1
- Prefix modifies command execution count
- Applies to repeatable commands only

Execution model:

for i in 1..count:
command()


---

## Buffer Operations

### Insert Char
- move gap if needed
- place char at gap_start
- increment gap_start

### Delete Forward
- expand gap_end

### Delete Backward
- decrement gap_start

### Move Cursor
- shift gap left/right

---

## Line Handling

- Lines separated by '\n'
- BOL/EOL found via scan
- Row/column computed on demand

---

## Mode Line Fields

MODE> [INS|CMD] row:X col:Y lines:N size:S dirty|clean


---

## Rendering

Always redraw:

1. TEXT line
   - current line only
   - insert `|` at point

2. MODE line

3. CMD line

---

## Memory Layout (conceptual)

0x000000 Editor code
0x010000 Source buffer
0x020000 Heap (gap buffer)
0x040000 Stack


---

## Future Hooks

- mark position (reserved, unused in MVP)
- kill buffer
- command registry for Lisp


