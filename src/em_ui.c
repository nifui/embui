#include <em/em_ui.h>
#include <em/em_tree.h>
#include <em/em_pool.h>
#include <em/em_event.h>

// Offer more raw capabilities and then add an abstraction layer over it so if some people don't
// wanna deal with the hassle of manually adding to a callback pool it's fine.

// The em_ctx is seperated from the UI, to allow it to be reused across multiple UIs.
typedef struct em_ui {
    em_tree*          tree;
    em_resource_pool* pool;
} em_ui;

int em_init_ui(struct em_ui* ui) {
}

em_result
em_add_prim(em_ctx* ctx, em_ui* ui, em_idx parent_idx, em_primitive_type type, em_handle* handle) {
    em_idx    handle_idx;
    em_result res;
    res = em_tree_add(ctx, ui->tree, parent_idx, &handle_idx);
    if (res != EM_OK) {
        return res;
    }
    res = em_add_handle(ctx, ui->pool, type, handle);
    if (res != EM_OK) {
        return res;
    }
    return EM_OK;
}

em_result em_remove_prim(em_ctx* ctx, em_ui* ui, em_handle* handle) {
    switch (handle->type) {
    case RECT:
        break;
    }
}

// Returns back the corresponding index at which the style was placed.

em_result em_add_style(em_ctx* ctx, em_ui* ui, em_style style, em_idx* style_idx) {
}

// Changes the style to the one at the given index.
em_result em_change_style(em_ctx* ctx, em_ui* ui, em_handle* handle, em_idx style_idx) {
}

em_result em_remove_element(em_ctx* ctx, em_ui* ui, em_idx target) {
}

// Replace the old style with a new style.
em_result em_modify_style(em_ui* ui, em_idx idx, em_style* new) {
}

em_result em_register_stateful(em_ui* ui, void* state, em_callback handler, em_tree* widget) {
}

// Whe emitting commands, it figures out how to layout the elements based on it's siblings and
// its parents. Children's tend not to inherent parents unless explicitly specified.
