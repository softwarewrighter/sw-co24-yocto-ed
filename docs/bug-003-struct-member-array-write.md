# tc24r BUG-003: Struct member array element write uses array contents as pointer

## Category
Compiler (codegen)

## Summary

Writing to an element of a char array that is a struct member (`s.arr[i] = val`)
generates code that loads the array's *contents* as a pointer, then dereferences
that pointer. If the array is zeroed (e.g. after memset), this writes to address
0 (the code segment), corrupting execution.

## Reproduce

```c
#define UART_DATA   0xFF0100
#define UART_STATUS 0xFF0101
static void uart_putc(int ch) { while (*(char *)UART_STATUS & 0x80) {} *(char *)UART_DATA = ch; }
static void uart_puts(const char *s) { while (*s) { uart_putc(*s); s = s + 1; } }
static void sye_memset(char *dst, int val, int n) { int i; for (i = 0; i < n; i = i + 1) dst[i] = val; }
struct S { char *base; int mode; int quit; char cmd[128]; int cmdlen; char status[128]; };
int main(void) {
    struct S e;
    sye_memset((char *)&e, 0, sizeof(struct S));
    e.mode = 0;
    e.status[0] = 'r';   /* CRASHES: writes to address 0 */
    e.status[1] = 'e';
    e.status[2] = 0;
    uart_puts("done\n");
    return 0;
}
```

```bash
tc24r test.c -o test.s && cor24-run --run test.s --speed 0 -n 100000
# Output: garbled bytes, "done" never prints
```

## Generated Assembly (bad)

The compiler generates this for `e.status[0] = 'r'`:

```asm
lc      r0,-128         ; offset of status field from fp
add     r0,fp           ; r0 = &e.status  (correct address)
lw      r0,0(r0)        ; BUG: loads 3 bytes of e.status as a pointer (0 after memset)
lc      r1,0            ; index = 0
add     r0,r1           ; r0 = 0 + 0 = NULL
...
sw      r0,0(r1)        ; writes to address 0 -> corrupts code segment
```

Expected: `lc r0,-128; add r0,fp; lc r1,0; add r0,r1; lc r2,114; sb r2,0(r0)`

## What Works

- Reading from struct member arrays (array-to-pointer decay for function args): OK
- Writing via a local pointer variable: OK
- Writing to top-level (non-struct) array elements: OK
- `&e.status[0]` to get pointer, then `p[i] = val`: OK

```c
/* WORKAROUND: use a local pointer */
char *p = e.status;
p[0] = 'r';
p[1] = 'e';
p[2] = 0;
```

## Impact

High. Any struct with a char array field (common in embedded C: command buffers,
status strings, names) will silently corrupt memory on element writes after
zero-initialization.

## Status

Open. Workaround: always use a local pointer for struct member array writes.
