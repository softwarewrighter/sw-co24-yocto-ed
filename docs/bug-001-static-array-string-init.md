# tc24r BUG-001: Static array init from string literal emits zeros

## Category
Compiler (codegen)

## Reproduce
```c
static const char msg[] = "hello";
int main(void) { return msg[0]; }
```
```bash
tc24r test.c -o test.s
```

## Actual
Assembly emits all zeros:
```asm
_msg:
        .byte   0
        .byte   0
        .byte   0
        .byte   0
        .byte   0
        .byte   0
```

## Expected
```asm
_msg:
        .byte   104,101,108,108,111,0
```

## Contrast: what works
String literals used as pointers (not static array initializers) work correctly:
```c
int main(void) { const char *s = "hello"; return s[0]; }
```
This correctly emits `_S0: .byte 104,101,108,108,111,0`.

## Impact
High. Many C programs initialize static data from string literals. This is the correct way to declare read-only string data in C89/C99.
