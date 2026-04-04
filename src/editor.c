#define SYE_CH_ESC 27
#define SYE_CH_C_RIGHT_BRACKET 29
#define SYE_CH_BACKSPACE_1 8
#define SYE_CH_BACKSPACE_2 127
#define SYE_CH_ENTER_1 '\n'
#define SYE_CH_ENTER_2 '\r'

static void sye_set_status(struct sye_editor *editor, const char *message) {
    int i;
    if (!editor || !message) {
        return;
    }
    for (i = 0; i < SYE_STATUS_BUF_CAP - 1 && message[i] != '\0'; i++) {
        editor->status_buffer[i] = message[i];
    }
    editor->status_buffer[i] = '\0';
}

void sye_editor_init(
    struct sye_editor *editor,
    char *work_buffer,
    int work_capacity,
    const char *initial_text
) {
    if (!editor) {
        return;
    }
    sye_memset((char *)editor, 0, sizeof(struct sye_editor));
    editor->mode = SYE_MODE_EDIT;
    sye_buffer_init(&editor->buffer, work_buffer, work_capacity);
    sye_buffer_load_cstr(&editor->buffer, initial_text ? initial_text : "");
    sye_set_status(editor, "ready");
}

void sye_editor_handle_edit_key(struct sye_editor *editor, int ch) {
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

void sye_editor_handle_command_key(struct sye_editor *editor, int ch) {
    struct sye_cmd_invocation invocation;
    int status;

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
            if (editor->command_length > 0) {
                editor->command_length -= 1;
                editor->command_buffer[editor->command_length] = '\0';
            }
            return;
        case SYE_CH_ENTER_1:
        case SYE_CH_ENTER_2:
            break;
        default:
            if (ch >= 32 && ch <= 126 && editor->command_length + 1 < SYE_CMD_BUF_CAP) {
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
        editor->quit_requested = 1;
    }

    editor->mode = SYE_MODE_EDIT;
    editor->command_length = 0;
    editor->command_buffer[0] = '\0';
}

void sye_editor_render(const struct sye_editor *editor) {
    char text_line[256];
    char mode_line[256];
    char command_line[256];

    if (!editor) {
        return;
    }

    sye_render_text_line(editor, text_line, 256);
    sye_render_mode_line(editor, mode_line, 256);
    sye_render_command_line(editor, command_line, 256);

    uart_puts("\n");
    uart_puts(text_line);
    uart_puts("\n");
    uart_puts(mode_line);
    uart_puts("\n");
    uart_puts(command_line);
    uart_puts("\n");
}
