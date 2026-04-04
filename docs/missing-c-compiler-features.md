# tc24r Bug Reports

## BUG-001: Static array initialization from string literal produces zeros

**Reproduce:**
```c
static const char msg[] = "hello";
int main(void) { return msg[0]; }
```
```bash
tc24r test.c -o test.s
```
Assembly output shows `_msg: .byte 0,0,0,0,0,0` -- all zeros.

**Expected:** `_msg: .byte 104,101,108,108,111,0` ("hello\0").

**Works:** String literals used directly (not as static array initializers):
```c
int main(void) { const char *s = "hello"; return s[0]; }
```
This correctly emits `_S0: .byte 104,101,108,108,111,0`.

**Impact:** High. Many C programs initialize static data from string literals.

---

## BUG-002: Rust panic -- address-of struct member (FIXED)

`&variable.member` caused a panic in `gen_addr_of`. This has been fixed.
See git history for original repro case.

---

## BUG-003: Array initializer list not supported

**Reproduce:**
```c
static int msg[] = {72, 105, 0};
int main(void) { return msg[0]; }
```
```bash
tc24r test.c -o test.s
```
**Actual:** `tc24r: error at offset 19: expected expression, got LBrace`

**Expected:** Compiles. Array initializer lists are C89.

**Impact:** Medium. Can work around with individual assignments, but verbose.

---

## BUG-003: Struct member array element write uses array contents as pointer

`s.arr[i] = val` generates code that loads the array's contents as a pointer
then dereferences it. If the array is zeroed (e.g. after memset), this writes
to address 0, corrupting the code segment.

**See:** `docs/bug-003-struct-member-array-write.md` for full repro and assembly.

**Workaround:** Use a local pointer variable:
```c
char *p = s.arr;
p[i] = val;  /* correct */
```

**Impact:** High. Silent memory corruption on any struct with char array fields.
