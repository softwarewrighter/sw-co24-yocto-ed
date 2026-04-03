#ifndef SW_YOCTO_ED_BUFFER_H
#define SW_YOCTO_ED_BUFFER_H

#include <stddef.h>
#include <stdbool.h>
#include "editor.h"

void sye_buffer_init(sye_buffer_t *buffer, char *storage, size_t capacity);
bool sye_buffer_load_cstr(sye_buffer_t *buffer, const char *text);
size_t sye_buffer_size(const sye_buffer_t *buffer);
size_t sye_buffer_point(const sye_buffer_t *buffer);
size_t sye_buffer_gap_size(const sye_buffer_t *buffer);

bool sye_buffer_insert_char(sye_buffer_t *buffer, char ch);
bool sye_buffer_insert_cstr(sye_buffer_t *buffer, const char *text);
bool sye_buffer_delete_backward_char(sye_buffer_t *buffer);
bool sye_buffer_delete_forward_char(sye_buffer_t *buffer);
bool sye_buffer_move_left(sye_buffer_t *buffer);
bool sye_buffer_move_right(sye_buffer_t *buffer);

size_t sye_buffer_copy_out(const sye_buffer_t *buffer, char *dest, size_t dest_capacity);
size_t sye_buffer_row(const sye_buffer_t *buffer);
size_t sye_buffer_col(const sye_buffer_t *buffer);
size_t sye_buffer_line_count(const sye_buffer_t *buffer);

#endif
