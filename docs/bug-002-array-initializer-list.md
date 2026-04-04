# tc24r BUG-002: Array initializer list not supported

## Category
Compiler (parser)

## Reproduce
```c
static int msg[] = {72, 105, 0};
int main(void) { return msg[0]; }
```
```bash
tc24r test.c -o test.s
```

## Actual
```
tc24r: error at offset 19: expected expression, got LBrace
```

## Expected
Compiles successfully. Array initializer lists with `{}` are standard C89.

## Impact
Medium. Can work around with individual element assignments, but verbose and can't be used for static/const data.
