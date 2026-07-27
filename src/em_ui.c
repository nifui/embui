#include <em/em_ui.h>
#include <em/em_tree.h>
#include <em/em_pool.h>
#include <em/em_event.h>
// The em_ctx is seperated from the UI, to allow it to be reused across multiple UIs.
typedef struct em_ui {
    em_tree          *tree;
    em_resource_pool *pool;
} em_ui;

int em_init_ui(struct em_ui *ui) {}

// For each element/widget types a function is defined that adds into the em_resource_pool and
// registers a handle it gets back from the tree after node creation. The handle is then returned
// for the user. When the user wants to add state they can register a void *state  with some sort of
// handle registry. If the specified node's handle detexts an event acting on it, it'll call a
// function that gets the void * state and then pass it into another function for the user to handle
// the event.
// That way the user never has to explictly perform a search up of the assocaited widget state.
// Since handle indices are stable most of the time we can use it as a sort of key into the
// associated states and callbacks.

// Returns back a handle to the resource for use. If the user wnats to delete an element they just
// provide the handle and the handle contains enough information to do so.
// Along with modifying the style of it can easily be done with said handle aswell and any other
// fields.
em_result
em_add_prim(em_ctx *ctx, em_ui *ui, em_idx parent_idx, em_primitive_type type, em_handle *handle) {
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
em_result em_remove_element(em_ctx *ctx, em_ui *ui, em_idx target) {}

em_idx em_add_rect(em_ui *ui, em_idx parent_idx) {}
em_idx em_add_line(em_ui *ui, em_idx parent_idx) {}
em_idx em_add_text(em_ui *ui, em_idx parent_idx) {}

// Every widget has a default style and by default will point to said default style. Those can be
// replaced.
em_idx em_modify_style(em_ui *ui, em_idx idx) {}

// Registers a stateful widget
// The widget can be represented as a subtree.
// Might be possible to use define macros to avoid having to do subtree operations which can be
// expensive depending on size.
// That way it get's directly manipulated instead of having to go through this layer.
int em_register_stateful(em_ui *ui, void *state, em_callback handler, em_tree *widget) {}

// Just take a mouse event and do a binary search basically. However this assumes that elements
// aren't just going to overlap with multiple elements. Along with that visibility of the element
// has to be tested as well.
int em_add_button() {}

// Whe emitting commands, it figures out how to layout the elements based on it's siblings and
// its parents. Children's tend not to inherent parents unless explicitly specified.
