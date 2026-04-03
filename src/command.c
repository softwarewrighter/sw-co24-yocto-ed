#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "command.h"

static const char *sye_skip_spaces(const char *p) {
    while (p && *p != '\0' && isspace((unsigned char)*p)) {
        ++p;
    }
    return p;
}

bool sye_command_parse(char *input, sye_command_invocation_t *invocation) {
    char *p;
    char *name_start;

    if (!input || !invocation) {
        return false;
    }

    invocation->repeat_count = 1U;
    invocation->name = "";
    invocation->args = "";

    p = input;
    p = (char *)sye_skip_spaces(p);
    if (*p == '\0') {
        return false;
    }

    if (isdigit((unsigned char)*p)) {
        invocation->repeat_count = (size_t)strtoul(p, &p, 10);
        if (invocation->repeat_count == 0U) {
            invocation->repeat_count = 1U;
        }
        p = (char *)sye_skip_spaces(p);
    }

    name_start = p;
    while (*p != '\0' && !isspace((unsigned char)*p)) {
        ++p;
    }

    if (*p != '\0') {
        *p++ = '\0';
    }

    invocation->name = name_start;
    p = (char *)sye_skip_spaces(p);
    invocation->args = p;
    return true;
}

static sye_status_t sye_execute_once(sye_editor_t *editor, const sye_command_invocation_t *invocation) {
    if (strcmp(invocation->name, "left") == 0) {
        return sye_buffer_move_left(&editor->buffer) ? SYE_STATUS_OK : SYE_STATUS_BOUNDARY;
    }
    if (strcmp(invocation->name, "right") == 0) {
        return sye_buffer_move_right(&editor->buffer) ? SYE_STATUS_OK : SYE_STATUS_BOUNDARY;
    }
    if (strcmp(invocation->name, "backdel") == 0) {
        return sye_buffer_delete_backward_char(&editor->buffer) ? SYE_STATUS_OK : SYE_STATUS_BOUNDARY;
    }
    if (strcmp(invocation->name, "del") == 0) {
        return sye_buffer_delete_forward_char(&editor->buffer) ? SYE_STATUS_OK : SYE_STATUS_BOUNDARY;
    }
    if (strcmp(invocation->name, "newline") == 0) {
        return sye_buffer_insert_char(&editor->buffer, '\n') ? SYE_STATUS_OK : SYE_STATUS_NO_SPACE;
    }
    if (strcmp(invocation->name, "insert") == 0) {
        return sye_buffer_insert_cstr(&editor->buffer, invocation->args) ? SYE_STATUS_OK : SYE_STATUS_NO_SPACE;
    }
    if (strcmp(invocation->name, "save") == 0) {
        editor->buffer.dirty = false;
        (void)snprintf(editor->status_buffer, sizeof(editor->status_buffer), "saved (stub)");
        return SYE_STATUS_OK;
    }
    if (strcmp(invocation->name, "new") == 0) {
        sye_buffer_init(&editor->buffer, editor->buffer.base, editor->buffer.capacity);
        (void)snprintf(editor->status_buffer, sizeof(editor->status_buffer), "new buffer");
        return SYE_STATUS_OK;
    }
    if (strcmp(invocation->name, "quit") == 0) {
        (void)snprintf(editor->status_buffer, sizeof(editor->status_buffer), "quit requested");
        return SYE_STATUS_QUIT;
    }
    if (strcmp(invocation->name, "help") == 0) {
        (void)snprintf(editor->status_buffer, sizeof(editor->status_buffer), "help: left right del backdel newline insert save new quit");
        return SYE_STATUS_OK;
    }

    (void)snprintf(editor->status_buffer, sizeof(editor->status_buffer), "unknown: %s", invocation->name);
    return SYE_STATUS_UNKNOWN_COMMAND;
}

sye_status_t sye_command_execute(sye_editor_t *editor, const sye_command_invocation_t *invocation) {
    size_t i;
    sye_status_t status = SYE_STATUS_OK;

    if (!editor || !invocation) {
        return SYE_STATUS_ERR;
    }

    for (i = 0U; i < invocation->repeat_count; ++i) {
        status = sye_execute_once(editor, invocation);
        if (status != SYE_STATUS_OK) {
            break;
        }
    }

    if (status == SYE_STATUS_OK && strcmp(invocation->name, "insert") != 0 && strcmp(invocation->name, "help") != 0 && strcmp(invocation->name, "save") != 0 && strcmp(invocation->name, "new") != 0) {
        (void)snprintf(editor->status_buffer, sizeof(editor->status_buffer), "ok");
    }

    return status;
}
