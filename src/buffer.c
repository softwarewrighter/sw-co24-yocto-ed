static char sye_buffer_char_at(const struct sye_buffer *buffer, int logical_index) {
    int gap_size;
    if (logical_index < buffer->gap_start) {
        return buffer->base[logical_index];
    }
    gap_size = buffer->gap_end - buffer->gap_start;
    return buffer->base[logical_index + gap_size];
}

void sye_buffer_init(struct sye_buffer *buffer, char *storage, int capacity) {
    if (!buffer) {
        return;
    }
    buffer->base = storage;
    buffer->capacity = capacity;
    buffer->gap_start = 0;
    buffer->gap_end = capacity;
    buffer->dirty = 0;
}

int sye_buffer_load_cstr(struct sye_buffer *buffer, const char *text) {
    int len;
    if (!buffer || !text) {
        return 0;
    }
    len = sye_strlen(text);
    if (len > buffer->capacity) {
        return 0;
    }
    sye_memcpy(buffer->base + (buffer->capacity - len), text, len);
    buffer->gap_start = 0;
    buffer->gap_end = buffer->capacity - len;
    buffer->dirty = 0;
    return 1;
}

int sye_buffer_size(const struct sye_buffer *buffer) {
    return buffer->capacity - (buffer->gap_end - buffer->gap_start);
}

int sye_buffer_point(const struct sye_buffer *buffer) {
    return buffer->gap_start;
}

int sye_buffer_gap_size(const struct sye_buffer *buffer) {
    return buffer->gap_end - buffer->gap_start;
}

int sye_buffer_insert_char(struct sye_buffer *buffer, char ch) {
    if (!buffer || sye_buffer_gap_size(buffer) == 0) {
        return 0;
    }
    buffer->base[buffer->gap_start] = ch;
    buffer->gap_start = buffer->gap_start + 1;
    buffer->dirty = 1;
    return 1;
}

int sye_buffer_insert_cstr(struct sye_buffer *buffer, const char *text) {
    int i;
    if (!buffer || !text) {
        return 0;
    }
    for (i = 0; text[i] != '\0'; i = i + 1) {
        if (!sye_buffer_insert_char(buffer, text[i])) {
            return 0;
        }
    }
    return 1;
}

int sye_buffer_delete_backward_char(struct sye_buffer *buffer) {
    if (!buffer || buffer->gap_start == 0) {
        return 0;
    }
    buffer->gap_start = buffer->gap_start - 1;
    buffer->dirty = 1;
    return 1;
}

int sye_buffer_delete_forward_char(struct sye_buffer *buffer) {
    if (!buffer || buffer->gap_end >= buffer->capacity) {
        return 0;
    }
    buffer->gap_end = buffer->gap_end + 1;
    buffer->dirty = 1;
    return 1;
}

int sye_buffer_move_left(struct sye_buffer *buffer) {
    if (!buffer || buffer->gap_start == 0) {
        return 0;
    }
    buffer->gap_start = buffer->gap_start - 1;
    buffer->gap_end = buffer->gap_end - 1;
    buffer->base[buffer->gap_end] = buffer->base[buffer->gap_start];
    return 1;
}

int sye_buffer_move_right(struct sye_buffer *buffer) {
    if (!buffer || buffer->gap_end >= buffer->capacity) {
        return 0;
    }
    buffer->base[buffer->gap_start] = buffer->base[buffer->gap_end];
    buffer->gap_start = buffer->gap_start + 1;
    buffer->gap_end = buffer->gap_end + 1;
    return 1;
}

int sye_buffer_copy_out(const struct sye_buffer *buffer, char *dest, int dest_capacity) {
    int size;
    int left_size;
    int right_size;
    if (!buffer || !dest || dest_capacity == 0) {
        return 0;
    }
    size = sye_buffer_size(buffer);
    if (size + 1 > dest_capacity) {
        size = dest_capacity - 1;
    }
    left_size = buffer->gap_start < size ? buffer->gap_start : size;
    sye_memcpy(dest, buffer->base, left_size);
    right_size = size - left_size;
    if (right_size > 0) {
        sye_memcpy(dest + left_size, buffer->base + buffer->gap_end, right_size);
    }
    dest[size] = '\0';
    return size;
}

int sye_buffer_row(const struct sye_buffer *buffer) {
    int row = 1;
    int i;
    if (!buffer) {
        return 0;
    }
    for (i = 0; i < sye_buffer_point(buffer); i = i + 1) {
        if (sye_buffer_char_at(buffer, i) == '\n') {
            row = row + 1;
        }
    }
    return row;
}

int sye_buffer_col(const struct sye_buffer *buffer) {
    int col = 0;
    int i;
    if (!buffer) {
        return 0;
    }
    for (i = 0; i < sye_buffer_point(buffer); i = i + 1) {
        if (sye_buffer_char_at(buffer, i) == '\n') {
            col = 0;
        } else {
            col = col + 1;
        }
    }
    return col;
}

int sye_buffer_line_count(const struct sye_buffer *buffer) {
    int lines = 1;
    int i;
    int size;
    if (!buffer) {
        return 0;
    }
    size = sye_buffer_size(buffer);
    if (size == 0) {
        return 1;
    }
    for (i = 0; i < size; i = i + 1) {
        if (sye_buffer_char_at(buffer, i) == '\n') {
            lines = lines + 1;
        }
    }
    return lines;
}
