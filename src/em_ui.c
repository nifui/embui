#include <em/em_ui.h>
#include <em/em_tree.h>

em_result em_init_ui(em_ctx* ctx, em_ui* ui, em_ui_desc* desc, em_handle* root_handle) {
    em_result res;
    res = em_tree_init(ctx, &ui->tree);
    EM_EXPECT(res);

    // Add onto the root node.
    EM_NODE_ROOT;

    return EM_OK;
}

// Provide a primitive to be added. Will not attempt to find similar primitives. For that another
// function can be used.
em_result em_add_el(em_ctx* ctx, em_ui* ui, em_handle parent_handle, em_prim primitive) {
}
