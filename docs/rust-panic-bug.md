# tc24r Rust Panic Bug: Address-of Struct Member

## Summary

tc24r panics with `no entry found for key` when computing the address of a struct member (`&variable.member`). Taking the address of the whole struct (`&variable`) works fine.

## Location

`tc24r-emit-load-store/src/load.rs:23:34` in `gen_addr_of`, called from `gen_local_decl`.

## Reproduce

```c
struct s { int x; int y; };
int bar(void) { struct s a; int *p = &a.y; return *p; }
int main(void) { return bar(); }
```

```bash
tc24r test.c -o test.s
```

**Actual:** Panic with stack trace:
```
thread 'main' panicked at .../tc24r-emit-load-store/src/load.rs:23:34:
no entry found for key
```

**Expected:** Compiles successfully. `&a.y` is valid C -- it computes the address of member `y` within struct `a`.

## Variations that also panic

```c
// Address of first member
struct s { int x; int y; };
int bar(void) { struct s a; int *p = &a.x; return *p; }

// Address of nested struct member
struct inner { int x; };
struct outer { struct inner b; int mode; };
int bar(void) { struct outer a; struct inner *p = &a.b; return p->x; }

// Address of deeply nested member
struct inner { int x; };
struct outer { struct inner b; int mode; };
int bar(void) { struct outer a; int *p = &a.b.x; return *p; }
```

## What works (no panic)

```c
// Address of whole struct
struct s { int x; int y; };
int foo(struct s *p) { return p->x; }
int bar(void) { struct s a; a.x = 42; return foo(&a); }

// Direct member access (no &)
struct s { int x; int y; };
int bar(void) { struct s a; a.x = 42; return a.x; }
```

## Impact

High. Passing `&editor->buffer` (address of a nested struct member) as a function argument is a fundamental C pattern. This blocks the editor project and most non-trivial C programs that use structs with pass-by-pointer.
