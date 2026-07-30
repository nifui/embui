#include <em/em_ui.h>
#include <em/em_tree.h>
#include <em/em_pool.h>
#include <em/em_event.h>
#include <em/em_internal.h>
#include <em/em_type.h>

// Offer more raw capabilities and then add an abstraction layer over it so if some people don't
// wanna deal with the hassle of manually adding to a callback pool it's fine.

// The em_ctx is seperated from the UI, to allow it to be reused across multiple UIs.

// This is the dumbest fix ever for a problem involving headers.
em_resources* em_get_resources(em_ui* ui) {
    return ui->resources;
}

int em_init_ui(struct em_ui* ui) {
}

// Adds a new primitive meaning a new em_prim is put into the prims field.
// To use a prim in two handles (sharing)
em_result
em_add_prim(em_ctx* ctx, em_ui* ui, em_idx parent_idx, em_idx style_idx, em_primitive_type type) {
    em_idx    tree_idx;
    em_idx    res_idx;
    em_result res;

    res = em_tree_add(ctx, ui->tree, parent_idx, &tree_idx);
    EM_EXPECT(res);

    res = em_add_resource(ctx, ui->resources, type, &res_idx);
    EM_EXPECT(res);

    res = em_add_handle(ctx,
                        ui->resources,
                        (em_handle){.tree_idx     = tree_idx,
                                    .prim_idx     = ui->resources->prims.size,
                                    .callback_idx = EM_IDX_NULL,
                                    .style_idx    = style_idx});
    EM_EXPECT(res);

    ui->tree->nodes.data[tree_idx].handle_idx = ui->resources->handles.size - 1;
    return EM_OK;
}

// Creates n clones of the same element sharing the same parent.
// Useful for array of similar elements. More of a helper function if anything.
em_result em_shared_prims(em_ctx* ctx, em_ui* ui, em_handle* reference, size_t shared_count) {

    em_result         res;
    em_idx            parent_idx = ui->tree->nodes.data[reference->tree_idx].parent;
    em_primitive_type prim_type  = ui->resources->prims.data[reference->prim_idx].type;

    if (parent_idx == EM_NODE_NULL) {
        return EM_ERR_INVALID_INDEX;
    }

    for (size_t i = 0; i < shared_count; i++) {
        res = em_add_prim(ctx, ui, parent_idx, reference->style_idx, prim_type);
        EM_EXPECT(res);
    }

    return EM_OK;
}

em_result em_remove_prim(em_ctx* ctx, em_ui* ui, em_handle* handle) {
}

// Returns back the corresponding index at which the style was placed.
// Whe emitting commands, it figures out how to layout the elements based on it's siblings and
// its parents. Cahildren's tend not to inherent parents unless explicitly specified.
