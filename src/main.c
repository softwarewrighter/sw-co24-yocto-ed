#include <stddef.h>
#include "editor.h"
#include "platform.h"

#define SYE_WORK_BUFFER_CAPACITY 4096U

static char g_work_buffer[SYE_WORK_BUFFER_CAPACITY];
static const char g_initial_text[] =
    "Welcome to sw-yocto-ed\n"
    "This is a bootstrap skeleton.\n";

int main(void) {
    sye_editor_t editor;
    sye_editor_init(&editor, g_work_buffer, SYE_WORK_BUFFER_CAPACITY, g_initial_text);
    sye_editor_render(&editor);

    while (!editor.quit_requested) {
        int ch = sye_platform_read_char();
        if (ch < 0) {
            continue;
        }

        if (editor.mode == SYE_MODE_EDIT) {
            sye_editor_handle_edit_key(&editor, ch);
        } else {
            sye_editor_handle_command_key(&editor, ch);
        }

        sye_editor_render(&editor);
    }

    return 0;
}
