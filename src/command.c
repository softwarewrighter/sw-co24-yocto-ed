static const char *sye_skip_spaces(const char *p) {
    while (p && *p != '\0' && sye_isspace(*p)) {
        p++;
    }
    return p;
}

int sye_command_parse(char *input, struct sye_cmd_invocation *invocation) {
    char *p;
    char *name_start;

    if (!input || !invocation) {
        return 0;
    }

    invocation->repeat_count = 1;
    invocation->name = "";
    invocation->args = "";

    p = input;
    p = sye_skip_spaces(p);
    if (*p == '\0') {
        return 0;
    }

    if (sye_isdigit(*p)) {
        invocation->repeat_count = sye_strtoul(p, &p);
        if (invocation->repeat_count == 0) {
            invocation->repeat_count = 1;
        }
        p = sye_skip_spaces(p);
    }

    name_start = (char *)p;
    while (*p != '\0' && !sye_isspace(*p)) {
        p++;
    }

    if (*p != '\0') {
        *p++ = '\0';
    }

    invocation->name = name_start;
    p = sye_skip_spaces(p);
    invocation->args = p;
    return 1;
}

static int sye_execute_once(struct sye_editor *editor, const struct sye_cmd_invocation *invocation) {
    if (sye_strcmp(invocation->name, "left") == 0) {
        return sye_buffer_move_left(&editor->buffer) ? SYE_STATUS_OK : SYE_STATUS_BOUNDARY;
    }
    if (sye_strcmp(invocation->name, "right") == 0) {
        return sye_buffer_move_right(&editor->buffer) ? SYE_STATUS_OK : SYE_STATUS_BOUNDARY;
    }
    if (sye_strcmp(invocation->name, "backdel") == 0) {
        return sye_buffer_delete_backward_char(&editor->buffer) ? SYE_STATUS_OK : SYE_STATUS_BOUNDARY;
    }
    if (sye_strcmp(invocation->name, "del") == 0) {
        return sye_buffer_delete_forward_char(&editor->buffer) ? SYE_STATUS_OK : SYE_STATUS_BOUNDARY;
    }
    if (sye_strcmp(invocation->name, "newline") == 0) {
        return sye_buffer_insert_char(&editor->buffer, '\n') ? SYE_STATUS_OK : SYE_STATUS_NO_SPACE;
    }
    if (sye_strcmp(invocation->name, "insert") == 0) {
        return sye_buffer_insert_cstr(&editor->buffer, invocation->args) ? SYE_STATUS_OK : SYE_STATUS_NO_SPACE;
    }
    if (sye_strcmp(invocation->name, "save") == 0) {
        editor->buffer.dirty = 0;
        sye_set_status_buf(editor->status_buffer, SYE_STATUS_BUF_CAP, "saved (stub)");
        return SYE_STATUS_OK;
    }
    if (sye_strcmp(invocation->name, "new") == 0) {
        sye_buffer_init(&editor->buffer, editor->buffer.base, editor->buffer.capacity);
        sye_set_status_buf(editor->status_buffer, SYE_STATUS_BUF_CAP, "new buffer");
        return SYE_STATUS_OK;
    }
    if (sye_strcmp(invocation->name, "quit") == 0) {
        return SYE_STATUS_QUIT;
    }
    if (sye_strcmp(invocation->name, "help") == 0) {
        sye_set_status_buf(editor->status_buffer, SYE_STATUS_BUF_CAP, "help: left right del backdel newline insert save new quit");
        return SYE_STATUS_OK;
    }

    return SYE_STATUS_UNKNOWN_COMMAND;
}

int sye_command_execute(struct sye_editor *editor, const struct sye_cmd_invocation *invocation) {
    int i;
    int status = SYE_STATUS_OK;

    if (!editor || !invocation) {
        return SYE_STATUS_ERR;
    }

    for (i = 0; i < invocation->repeat_count; i++) {
        status = sye_execute_once(editor, invocation);
        if (status != SYE_STATUS_OK) {
            break;
        }
    }

    return status;
}
