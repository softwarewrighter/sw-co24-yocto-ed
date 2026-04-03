#ifndef SW_YOCTO_ED_RENDER_H
#define SW_YOCTO_ED_RENDER_H

#include <stddef.h>
#include "editor.h"

void sye_render_text_line(const sye_editor_t *editor, char *out, size_t out_capacity);
void sye_render_mode_line(const sye_editor_t *editor, char *out, size_t out_capacity);
void sye_render_command_line(const sye_editor_t *editor, char *out, size_t out_capacity);

#endif
