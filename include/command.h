#ifndef SW_YOCTO_ED_COMMAND_H
#define SW_YOCTO_ED_COMMAND_H

#include <stddef.h>
#include <stdbool.h>
#include "editor.h"

typedef struct {
    size_t repeat_count;
    const char *name;
    const char *args;
} sye_command_invocation_t;

bool sye_command_parse(char *input, sye_command_invocation_t *invocation);
sye_status_t sye_command_execute(sye_editor_t *editor, const sye_command_invocation_t *invocation);

#endif
