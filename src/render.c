static char sye_buffer_char_at_render(const struct sye_buffer *buffer, int logical_index) {
    int gap_size;
    if (logical_index < buffer->gap_start) {
        return buffer->base[logical_index];
    }
    gap_size = buffer->gap_end - buffer->gap_start;
    return buffer->base[logical_index + gap_size];
}

static int sye_copy_str(char *dst, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return i;
}

static void sye_append_str(char *dst, int pos, int max, const char *src) {
    while (*src && pos < max - 1) {
        dst[pos++] = *src;
        src++;
    }
    dst[pos] = '\0';
}

static int sye_itoa(char *buf, int n) {
    int i = 0;
    if (n < 0) {
        buf[i++] = '-';
        n = 0 - n;
    }
    if (n == 0) {
        buf[i++] = '0';
    } else {
        char tmp[8];
        int t = 0;
        while (n > 0) {
            tmp[t++] = '0' + (n % 10);
            n = n / 10;
        }
        while (t > 0) {
            buf[i++] = tmp[--t];
        }
    }
    buf[i] = '\0';
    return i;
}

void sye_render_text_line(const struct sye_editor *editor, char *out, int out_capacity) {
    int size;
    int point;
    int bol;
    int eol;
    int i;
    int pos;

    if (!editor || !out || out_capacity == 0) {
        return;
    }

    size = sye_buffer_size(&editor->buffer);
    point = sye_buffer_point(&editor->buffer);
    bol = point;
    while (bol > 0 && sye_buffer_char_at_render(&editor->buffer, bol - 1) != '\n') {
        bol--;
    }

    eol = point;
    while (eol < size && sye_buffer_char_at_render(&editor->buffer, eol) != '\n') {
        eol++;
    }

    pos = sye_copy_str(out, "TEXT> ");

    for (i = bol; i <= eol && pos + 2 < out_capacity; i++) {
        if (i == point) {
            out[pos++] = '|';
        }
        if (i < eol) {
            out[pos++] = sye_buffer_char_at_render(&editor->buffer, i);
        }
    }

    out[pos] = '\0';
}

void sye_render_mode_line(const struct sye_editor *editor, char *out, int out_capacity) {
    int pos;
    char num[8];

    if (!editor || !out || out_capacity == 0) {
        return;
    }

    pos = sye_copy_str(out, "MODE> ");
    if (editor->mode == SYE_MODE_EDIT) {
        pos += sye_copy_str(out + pos, "INS");
    } else {
        pos += sye_copy_str(out + pos, "CMD");
    }

    pos += sye_copy_str(out + pos, " row:");
    pos += sye_itoa(out + pos, sye_buffer_row(&editor->buffer));

    pos += sye_copy_str(out + pos, " col:");
    pos += sye_itoa(out + pos, sye_buffer_col(&editor->buffer));

    pos += sye_copy_str(out + pos, " lines:");
    pos += sye_itoa(out + pos, sye_buffer_line_count(&editor->buffer));

    pos += sye_copy_str(out + pos, " size:");
    pos += sye_itoa(out + pos, sye_buffer_size(&editor->buffer));

    pos += sye_copy_str(out + pos, " ");
    if (editor->buffer.dirty) {
        sye_copy_str(out + pos, "dirty");
    } else {
        sye_copy_str(out + pos, "clean");
    }
}

void sye_render_command_line(const struct sye_editor *editor, char *out, int out_capacity) {
    if (!editor || !out || out_capacity == 0) {
        return;
    }

    sye_copy_str(out, "CMD > ");
    if (editor->mode == SYE_MODE_COMMAND) {
        sye_append_str(out, 6, out_capacity, editor->command_buffer);
    } else {
        sye_append_str(out, 6, out_capacity, editor->status_buffer);
    }
}
