#include "compat.h"

#define SYE_MODE_EDIT    0
#define SYE_MODE_COMMAND 1

#define SYE_STATUS_OK             0
#define SYE_STATUS_ERR            1
#define SYE_STATUS_BOUNDARY       2
#define SYE_STATUS_NO_SPACE       3
#define SYE_STATUS_QUIT           4
#define SYE_STATUS_UNKNOWN_COMMAND 5

#define SYE_CMD_BUF_CAP    128
#define SYE_STATUS_BUF_CAP 128

struct sye_buffer {
    char *base;
    int capacity;
    int gap_start;
    int gap_end;
    int dirty;
};

struct sye_cmd_invocation {
    int repeat_count;
    const char *name;
    const char *args;
};

struct sye_editor {
    struct sye_buffer buffer;
    int mode;
    int quit_requested;
    char command_buffer[SYE_CMD_BUF_CAP];
    int command_length;
    char status_buffer[SYE_STATUS_BUF_CAP];
};

static void sye_set_status_buf(char *buf, int cap, const char *msg) {
    int i;
    for (i = 0; i < cap - 1 && msg[i] != '\0'; i++) {
        buf[i] = msg[i];
    }
    buf[i] = '\0';
}

#include "buffer.c"
#include "command.c"
#include "editor.c"
#include "render.c"

#define SYE_WORK_BUF_CAP 4096
#define SYE_FILE_ADDR    0x10000

static char g_work_buffer[SYE_WORK_BUF_CAP];

int main(void) {
    struct sye_editor editor;
    sye_editor_init(&editor, g_work_buffer, SYE_WORK_BUF_CAP, (const char *)SYE_FILE_ADDR);
    sye_editor_render(&editor);

    while (!editor.quit_requested) {
        int ch = uart_getc();
        if (ch < 0) {
            continue;
        }
        if (editor.mode == SYE_MODE_EDIT) {
            sye_editor_handle_edit_key(&editor, ch);
        } else {
            sye_editor_handle_command_key(&editor, ch);
        }
        sye_editor_render(&editor);
    }

    return 0;
}
