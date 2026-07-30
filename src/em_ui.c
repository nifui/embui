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
em_result em_add_prim(em_ctx* ctx, em_ui* ui, em_idx parent_idx, em_primitive_type type) {
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
                                    .style_idx    = DEFAULT_STYLE_IDX});
    EM_EXPECT(res);

    ui->tree->nodes.data[tree_idx].handle_idx = ui->resources->handles.size - 1;
    return EM_OK;
}

// Assumed that the shared prim's are siblings.
em_result em_shared_prims(em_ctx*    ctx,
                          em_ui*     ui,
                          em_handle* reference,
                          size_t     shared_count,
                          em_handle* dst_handles,
                          em_idx*    tree_idx) {

    em_result res;

    for (size_t i = 0; i < shared_count; i++) {
        // What should be copied?
        // The tree_idx should always be unique .
        // Styling is shared because its required but callbacks aren't so they aren't shared.
        em_handle handle = {.prim_idx     = reference->prim_idx,
                            .style_idx    = reference->style_idx,
                            .tree_idx     = tree_idx[i],
                            .callback_idx = EM_IDX_NULL};
        // The handles are guaranteed contiguous so just return the start_idx and the user can
        // calculate via shared_count + start_idx.
        res = em_add_handle(ctx, ui->resources, handle);

        EM_EXPECT(res);
    }
}

em_result em_remove_prim(em_ctx* ctx, em_ui* ui, em_handle* handle) {
}

// Returns back the corresponding index at which the style was placed.
// Whe emitting commands, it figures out how to layout the elements based on it's siblings and
// its parents. Cahildren's tend not to inherent parents unless explicitly specified.
