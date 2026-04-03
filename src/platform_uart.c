#include "platform.h"

#define UART_DATA   0xFF0100
#define UART_STATUS 0xFF0101

int sye_platform_read_char(void) {
    while (!(*(char *)UART_STATUS & 0x01)) {}
    return *(char *)UART_DATA;
}

void sye_platform_write_char(char ch) {
    while (*(char *)UART_STATUS & 0x80) {}
    *(char *)UART_DATA = ch;
}

void sye_platform_write_string(const char *text) {
    while (*text) {
        sye_platform_write_char(*text);
        text = text + 1;
    }
}
