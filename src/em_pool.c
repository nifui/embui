#include <em/em_ui.h>
#include <em/em_type.h>
#include <em/em_pool.h>
#include <em/em_internal.h>

em_result em_pool_init(em_ctx* ctx, em_resources* pool) {
}

em_result em_create_handle(em_ctx*           ctx,
                           em_resources*     resource_pool,
                           em_primitive_type type,
                           em_idx            tree_idx) {
    em_prim prim = {
        .type = type,
    };
    em_handle handle = {
        .style_idx    = DEFAULT_STYLE_IDX,
        .tree_idx     = tree_idx,
        .callback_idx = EM_IDX_NULL,

    };
}

em_result em_modify_style(em_resources* resources, em_idx style_idx, em_style style) {
}

em_result em_add_style(em_ctx* ctx, em_resources* resources, em_style* style, em_idx* dst_idx) {
}

em_result em_change_style(em_ctx* ctx, em_resources* resources, em_idx target) {
}
