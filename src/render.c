#include <stdio.h>
#include <string.h>
#include "buffer.h"
#include "render.h"

static char sye_buffer_char_at_render(const sye_buffer_t *buffer, size_t logical_index) {
    size_t gap_size;
    if (logical_index < buffer->gap_start) {
        return buffer->base[logical_index];
    }

    gap_size = buffer->gap_end - buffer->gap_start;
    return buffer->base[logical_index + gap_size];
}

void sye_render_text_line(const sye_editor_t *editor, char *out, size_t out_capacity) {
    size_t size;
    size_t point;
    size_t bol;
    size_t eol;
    size_t i;
    size_t pos;

    if (!editor || !out || out_capacity == 0U) {
        return;
    }

    size = sye_buffer_size(&editor->buffer);
    point = sye_buffer_point(&editor->buffer);
    bol = point;
    while (bol > 0U && sye_buffer_char_at_render(&editor->buffer, bol - 1U) != '\n') {
        --bol;
    }

    eol = point;
    while (eol < size && sye_buffer_char_at_render(&editor->buffer, eol) != '\n') {
        ++eol;
    }

    pos = 0U;
    pos += (size_t)snprintf(out + pos, out_capacity - pos, "TEXT> ");

    for (i = bol; i <= eol && pos + 2U < out_capacity; ++i) {
        if (i == point) {
            out[pos++] = '|';
        }
        if (i < eol) {
            out[pos++] = sye_buffer_char_at_render(&editor->buffer, i);
        }
    }

    out[pos] = '\0';
}

void sye_render_mode_line(const sye_editor_t *editor, char *out, size_t out_capacity) {
    const char *mode_name;
    if (!editor || !out || out_capacity == 0U) {
        return;
    }

    mode_name = editor->mode == SYE_MODE_EDIT ? "INS" : "CMD";
    (void)snprintf(
        out,
        out_capacity,
        "MODE> %s row:%zu col:%zu lines:%zu size:%zu %s",
        mode_name,
        sye_buffer_row(&editor->buffer),
        sye_buffer_col(&editor->buffer),
        sye_buffer_line_count(&editor->buffer),
        sye_buffer_size(&editor->buffer),
        editor->buffer.dirty ? "dirty" : "clean"
    );
}

void sye_render_command_line(const sye_editor_t *editor, char *out, size_t out_capacity) {
    if (!editor || !out || out_capacity == 0U) {
        return;
    }

    if (editor->mode == SYE_MODE_COMMAND) {
        (void)snprintf(out, out_capacity, "CMD > %s", editor->command_buffer);
    } else {
        (void)snprintf(out, out_capacity, "CMD > %s", editor->status_buffer);
    }
}
