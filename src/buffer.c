#include <ctype.h>
#include <string.h>
#include "buffer.h"

static char sye_buffer_char_at(const sye_buffer_t *buffer, size_t logical_index) {
    size_t gap_size;
    if (logical_index < buffer->gap_start) {
        return buffer->base[logical_index];
    }

    gap_size = buffer->gap_end - buffer->gap_start;
    return buffer->base[logical_index + gap_size];
}

void sye_buffer_init(sye_buffer_t *buffer, char *storage, size_t capacity) {
    if (!buffer) {
        return;
    }

    buffer->base = storage;
    buffer->capacity = capacity;
    buffer->gap_start = 0U;
    buffer->gap_end = capacity;
    buffer->dirty = false;
}

bool sye_buffer_load_cstr(sye_buffer_t *buffer, const char *text) {
    size_t len;
    if (!buffer || !text) {
        return false;
    }

    len = strlen(text);
    if (len > buffer->capacity) {
        return false;
    }

    memcpy(buffer->base, text, len);
    buffer->gap_start = len;
    buffer->gap_end = buffer->capacity;
    buffer->dirty = false;
    return true;
}

size_t sye_buffer_size(const sye_buffer_t *buffer) {
    return buffer->capacity - (buffer->gap_end - buffer->gap_start);
}

size_t sye_buffer_point(const sye_buffer_t *buffer) {
    return buffer->gap_start;
}

size_t sye_buffer_gap_size(const sye_buffer_t *buffer) {
    return buffer->gap_end - buffer->gap_start;
}

bool sye_buffer_insert_char(sye_buffer_t *buffer, char ch) {
    if (!buffer || sye_buffer_gap_size(buffer) == 0U) {
        return false;
    }

    buffer->base[buffer->gap_start++] = ch;
    buffer->dirty = true;
    return true;
}

bool sye_buffer_insert_cstr(sye_buffer_t *buffer, const char *text) {
    size_t i;
    if (!buffer || !text) {
        return false;
    }

    for (i = 0U; text[i] != '\0'; ++i) {
        if (!sye_buffer_insert_char(buffer, text[i])) {
            return false;
        }
    }

    return true;
}

bool sye_buffer_delete_backward_char(sye_buffer_t *buffer) {
    if (!buffer || buffer->gap_start == 0U) {
        return false;
    }

    buffer->gap_start -= 1U;
    buffer->dirty = true;
    return true;
}

bool sye_buffer_delete_forward_char(sye_buffer_t *buffer) {
    if (!buffer || buffer->gap_end >= buffer->capacity) {
        return false;
    }

    buffer->gap_end += 1U;
    buffer->dirty = true;
    return true;
}

bool sye_buffer_move_left(sye_buffer_t *buffer) {
    if (!buffer || buffer->gap_start == 0U) {
        return false;
    }

    buffer->base[--buffer->gap_end] = buffer->base[--buffer->gap_start];
    return true;
}

bool sye_buffer_move_right(sye_buffer_t *buffer) {
    if (!buffer || buffer->gap_end >= buffer->capacity) {
        return false;
    }

    buffer->base[buffer->gap_start++] = buffer->base[buffer->gap_end++];
    return true;
}

size_t sye_buffer_copy_out(const sye_buffer_t *buffer, char *dest, size_t dest_capacity) {
    size_t size;
    size_t left_size;
    size_t right_size;

    if (!buffer || !dest || dest_capacity == 0U) {
        return 0U;
    }

    size = sye_buffer_size(buffer);
    if (size + 1U > dest_capacity) {
        size = dest_capacity - 1U;
    }

    left_size = buffer->gap_start < size ? buffer->gap_start : size;
    memcpy(dest, buffer->base, left_size);

    right_size = size - left_size;
    if (right_size > 0U) {
        memcpy(dest + left_size, buffer->base + buffer->gap_end, right_size);
    }

    dest[size] = '\0';
    return size;
}

size_t sye_buffer_row(const sye_buffer_t *buffer) {
    size_t row = 1U;
    size_t i;
    if (!buffer) {
        return 0U;
    }

    for (i = 0U; i < sye_buffer_point(buffer); ++i) {
        if (sye_buffer_char_at(buffer, i) == '\n') {
            row += 1U;
        }
    }

    return row;
}

size_t sye_buffer_col(const sye_buffer_t *buffer) {
    size_t col = 0U;
    size_t i;
    if (!buffer) {
        return 0U;
    }

    for (i = 0U; i < sye_buffer_point(buffer); ++i) {
        if (sye_buffer_char_at(buffer, i) == '\n') {
            col = 0U;
        } else {
            col += 1U;
        }
    }

    return col;
}

size_t sye_buffer_line_count(const sye_buffer_t *buffer) {
    size_t lines = 1U;
    size_t i;
    size_t size;

    if (!buffer) {
        return 0U;
    }

    size = sye_buffer_size(buffer);
    if (size == 0U) {
        return 1U;
    }

    for (i = 0U; i < size; ++i) {
        if (sye_buffer_char_at(buffer, i) == '\n') {
            lines += 1U;
        }
    }

    return lines;
}
