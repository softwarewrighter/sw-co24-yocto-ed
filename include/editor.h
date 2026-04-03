#ifndef SW_YOCTO_ED_EDITOR_H
#define SW_YOCTO_ED_EDITOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SYE_CMD_BUF_CAP 128U
#define SYE_STATUS_BUF_CAP 128U

typedef enum {
    SYE_MODE_EDIT = 0,
    SYE_MODE_COMMAND = 1
} sye_mode_t;

typedef enum {
    SYE_STATUS_OK = 0,
    SYE_STATUS_ERR = 1,
    SYE_STATUS_BOUNDARY = 2,
    SYE_STATUS_NO_SPACE = 3,
    SYE_STATUS_QUIT = 4,
    SYE_STATUS_UNKNOWN_COMMAND = 5
} sye_status_t;

typedef struct {
    char *base;
    size_t capacity;
    size_t gap_start;
    size_t gap_end;
    bool dirty;
} sye_buffer_t;

typedef struct {
    sye_buffer_t buffer;
    sye_mode_t mode;
    bool quit_requested;
    char command_buffer[SYE_CMD_BUF_CAP];
    size_t command_length;
    char status_buffer[SYE_STATUS_BUF_CAP];
} sye_editor_t;

void sye_editor_init(
    sye_editor_t *editor,
    char *work_buffer,
    size_t work_capacity,
    const char *initial_text
);

void sye_editor_render(const sye_editor_t *editor);
void sye_editor_handle_edit_key(sye_editor_t *editor, int ch);
void sye_editor_handle_command_key(sye_editor_t *editor, int ch);

#endif
