#include <stdio.h>
#include <string.h>
#include "editor.h"
#include "buffer.h"
#include "command.h"
#include "platform.h"
#include "render.h"

#define SYE_CH_ESC 27
#define SYE_CH_C_RIGHT_BRACKET 29
#define SYE_CH_BACKSPACE_1 8
#define SYE_CH_BACKSPACE_2 127
#define SYE_CH_ENTER_1 '\n'
#define SYE_CH_ENTER_2 '\r'

static void sye_set_status(sye_editor_t *editor, const char *message) {
    if (!editor || !message) {
        return;
    }

    (void)snprintf(editor->status_buffer, sizeof(editor->status_buffer), "%s", message);
}

void sye_editor_init(
    sye_editor_t *editor,
    char *work_buffer,
    size_t work_capacity,
    const char *initial_text
) {
    if (!editor) {
        return;
    }

    memset(editor, 0, sizeof(*editor));
    editor->mode = SYE_MODE_EDIT;
    sye_buffer_init(&editor->buffer, work_buffer, work_capacity);
    (void)sye_buffer_load_cstr(&editor->buffer, initial_text ? initial_text : "");
    sye_set_status(editor, "ready");
}

void sye_editor_handle_edit_key(sye_editor_t *editor, int ch) {
    if (!editor) {
        return;
    }

    switch (ch) {
        case SYE_CH_ESC:
            editor->mode = SYE_MODE_COMMAND;
            editor->command_length = 0;
            editor->command_buffer[0] = '\0';
            sye_set_status(editor, "command mode");
            break;
        case SYE_CH_ENTER_1:
        case SYE_CH_ENTER_2:
            if (!sye_buffer_insert_char(&editor->buffer, '\n')) {
                sye_set_status(editor, "no space for newline");
            }
            break;
        case SYE_CH_BACKSPACE_1:
        case SYE_CH_BACKSPACE_2:
            if (!sye_buffer_delete_backward_char(&editor->buffer)) {
                sye_set_status(editor, "start of buffer");
            }
            break;
        default:
            if (ch >= 32 && ch <= 126) {
                if (!sye_buffer_insert_char(&editor->buffer, (char)ch)) {
                    sye_set_status(editor, "no space for char");
                }
            }
            break;
    }
}

void sye_editor_handle_command_key(sye_editor_t *editor, int ch) {
    sye_command_invocation_t invocation;
    sye_status_t status;

    if (!editor) {
        return;
    }

    switch (ch) {
        case SYE_CH_C_RIGHT_BRACKET:
            editor->mode = SYE_MODE_EDIT;
            sye_set_status(editor, "edit mode");
            return;
        case SYE_CH_BACKSPACE_1:
        case SYE_CH_BACKSPACE_2:
            if (editor->command_length > 0U) {
                editor->command_length -= 1U;
                editor->command_buffer[editor->command_length] = '\0';
            }
            return;
        case SYE_CH_ENTER_1:
        case SYE_CH_ENTER_2:
            break;
        default:
            if (ch >= 32 && ch <= 126 && editor->command_length + 1U < sizeof(editor->command_buffer)) {
                editor->command_buffer[editor->command_length++] = (char)ch;
                editor->command_buffer[editor->command_length] = '\0';
            }
            return;
    }

    if (!sye_command_parse(editor->command_buffer, &invocation)) {
        sye_set_status(editor, "parse error");
        editor->mode = SYE_MODE_EDIT;
        editor->command_length = 0;
        editor->command_buffer[0] = '\0';
        return;
    }

    status = sye_command_execute(editor, &invocation);
    if (status == SYE_STATUS_QUIT) {
        editor->quit_requested = true;
    }

    editor->mode = SYE_MODE_EDIT;
    editor->command_length = 0;
    editor->command_buffer[0] = '\0';
}

void sye_editor_render(const sye_editor_t *editor) {
    char text_line[256];
    char mode_line[256];
    char command_line[256];

    if (!editor) {
        return;
    }

    sye_render_text_line(editor, text_line, sizeof(text_line));
    sye_render_mode_line(editor, mode_line, sizeof(mode_line));
    sye_render_command_line(editor, command_line, sizeof(command_line));

    sye_platform_write_string("\n");
    sye_platform_write_string(text_line);
    sye_platform_write_string("\n");
    sye_platform_write_string(mode_line);
    sye_platform_write_string("\n");
    sye_platform_write_string(command_line);
    sye_platform_write_string("\n");
}
