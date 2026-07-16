#include <stdbool.h>
#include "temp.h"

/*
What this does:
    - Keeps track of element depth, order, styling and visibility
    -
What this does not do:
    - Draw/render
    - Provide an OS-Window
    - Provide a font manager
*/
// Temporary implementation
bool t_vec_intersects_rect(t_vec2 vec, t_rect rect) {
    return false;
}

// This might not be needed considering we have a depth tree that naturally leads to commands being
// emitted in the correct order. Could be used elsewhere though.
t_rect t_rects_overlap(t_rect* a, t_rect* b) {
    return (t_rect){0, 0, 0, 0};
}

void t_init_ctx(t_ctx* ctx,
                void* (*malloc)(size_t size),
                void* (*realloc)(void* ptr, size_t size)) {
    ctx->malloc  = malloc;
    ctx->realloc = realloc;
}

t_cmd* t_emit_cmds(t_ctx* ctx) {

    return NULL;
}

void t_process_window_events(t_ctx* ctx, t_mouse_event* events, int event_count) {
    for (int i = 0; i < event_count; i++) {
        t_mouse_event event = events[i];
    }
}

// Compares the previous command list and new one and emits only the requried changes. This just
// takes the bookkeeping away from the user.
// CASES:
// The ID of the elements are the same but the other fields differ, indicates that the field needs
// to be updated so it emits that.
// There should not be any duplicated commands.
// This is useless
t_cmd* t_get_cmd_diffs(t_cmd* old, t_cmd* new, size_t old_size, size_t new_size) {
    size_t old_ptr = 0;
    size_t new_ptr = 0;
    for (; old_ptr < old_size; old_ptr++) {
    }
}

int main() {
}
