#define UART_DATA   0xFF0100
#define UART_STATUS 0xFF0101

static void uart_putc(int ch) {
    while (*(char *)UART_STATUS & 0x80) {}
    *(char *)UART_DATA = ch;
}

static void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s);
        s = s + 1;
    }
}

static int uart_getc(void) {
    while (!(*(char *)UART_STATUS & 0x01)) {}
    return *(char *)UART_DATA;
}

static void sye_memcpy(char *dst, const char *src, int n) {
    int i;
    for (i = 0; i < n; i = i + 1) {
        dst[i] = src[i];
    }
}

static void sye_memset(char *dst, int val, int n) {
    int i;
    for (i = 0; i < n; i = i + 1) {
        dst[i] = val;
    }
}

static int sye_strlen(const char *s) {
    int n = 0;
    while (*s) {
        n = n + 1;
        s = s + 1;
    }
    return n;
}

static int sye_strcmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a = a + 1;
        b = b + 1;
    }
    return *a - *b;
}

static int sye_isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static int sye_isdigit(int c) {
    return c >= '0' && c <= '9';
}

static int sye_strtoul(const char *s, const char **endp) {
    int n = 0;
    while (*s >= '0' && *s <= '9') {
        n = n * 10 + (*s - '0');
        s = s + 1;
    }
    if (endp) {
        *endp = s;
    }
    return n;
}
