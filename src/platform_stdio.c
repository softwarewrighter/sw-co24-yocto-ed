#include <stdio.h>
#include "platform.h"

int sye_platform_read_char(void) {
    return getchar();
}

void sye_platform_write_string(const char *text) {
    fputs(text, stdout);
    fflush(stdout);
}

void sye_platform_write_char(char ch) {
    fputc((int)ch, stdout);
    fflush(stdout);
}
