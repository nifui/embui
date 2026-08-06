#include <em/em_ui.h>
#include <em/em_event.h>

// These don't account/support null handles. Important to avoid forcing the user to store the handle
// for every item.
em_result em_add_descriptor(em_ctx* ctx, em_ui* ui, em_res_desc descriptor, em_handle* handle) {
    em_result res;

    res = em_reserve(ctx,
                     (void**)&ui->descriptors.data,
                     &ui->descriptors.capacity,
                     ui->descriptors.size + 1,
                     sizeof(em_res_desc));
    EM_EXPECT(res);

    *handle = ui->descriptors.size;

    EM_PUSH(ui->descriptors, descriptor);
    return EM_OK;
}

em_result em_init_ui(em_ctx* ctx, em_ui* ui, em_handle* root_handle, int width, int height) {
    em_result res;
    // Placeholder add a way to specify the size of the default allocations.
    res = em_tree_init(ctx, &ui->tree);
    EM_EXPECT(res);

    em_idx prim_idx;
    res = em_add_primitive(
        ctx,
        &ui->resources.primitives,
        (em_primitive){.r    = (em_rect){.x = 0, .y = 0, .height = height, .width = width},
                       .type = RECT},
        &prim_idx);
    EM_EXPECT(res);

    em_res_desc descriptor = (em_res_desc){
        .style_idx     = EM_TAG_DEFAULT,
        .tree_idx      = EM_NODE_ROOT,
        .callback_idx  = EM_IDX_NULL,
        .primitive_idx = prim_idx,
    };

    res = em_add_descriptor(ctx, ui, descriptor, root_handle);
    EM_EXPECT(res);

    return EM_OK;
}

// These don't account/support null handles. Important to avoid forcing the user to store the handle
// for every item.
em_result em_create_element(em_ctx*      ctx,
                            em_ui*       ui,
                            em_idx       target_idx,
                            em_primitive primitive,
                            em_handle*   handle) {
    em_result res;
    em_idx    prim_idx;
    em_idx    tree_idx;

    res = em_add_primitive(ctx, &ui->resources.primitives, primitive, &prim_idx);
    EM_EXPECT(res);

    res = em_tree_add(ctx, &ui->tree, target_idx, &tree_idx);
    EM_EXPECT(res);

    em_res_desc descriptor = (em_res_desc){.style_idx     = EM_TAG_DEFAULT,
                                           .tree_idx      = tree_idx,
                                           .callback_idx  = EM_IDX_NULL,
                                           .primitive_idx = prim_idx};

    res = em_add_descriptor(ctx, ui, descriptor, handle);
    EM_EXPECT(res);

    ui->tree.nodes.data[descriptor.tree_idx].handle_idx = *handle;

    return EM_OK;
}

em_result em_create_container(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle) {
    em_result res;
    em_idx    tree_idx;

    res = em_tree_add(ctx, &ui->tree, target_idx, &tree_idx);
    EM_EXPECT(res);

    em_res_desc descriptor = (em_res_desc){.style_idx     = EM_TAG_DEFAULT,
                                           .tree_idx      = tree_idx,
                                           .callback_idx  = EM_IDX_NULL,
                                           .primitive_idx = EM_IDX_NULL};

    res = em_add_descriptor(ctx, ui, descriptor, handle);
    EM_EXPECT(res);

    ui->tree.nodes.data[descriptor.tree_idx].handle_idx = *handle;

    return EM_OK;
}

em_result em_add_rect(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_rect rect) {
    em_primitive prim = {.type = RECT, .r = rect};
    return em_create_element(ctx, ui, target_idx, prim, handle);
}

em_result em_add_text(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_text text) {
    em_primitive prim = {.type = TEXT, .t = text};
    return em_create_element(ctx, ui, target_idx, prim, handle);
}

em_result em_add_line(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_line line) {
    em_primitive prim = {.type = LINE, .l = line};
    return em_create_element(ctx, ui, target_idx, prim, handle);
}

em_result
em_add_circle(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_circle circle) {
    em_primitive prim = {.type = CIRCLE, .c = circle};
    return em_create_element(ctx, ui, target_idx, prim, handle);
}

em_result
em_add_point(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_point point) {
    em_primitive prim = {.type = POINT, .p = point};
    return em_create_element(ctx, ui, target_idx, prim, handle);
}

const char* button_text = "Press me!";

EM_BOOL em_button_cb(void* state, em_event event) {
    em_button_state* button = (em_button_state*)state;
    switch (event.type) {
    default:
        button->pressed = !button->pressed;
        return EM_TRUE;
    }
}

// Should we return the single top most handle or all handles to allow the user to easily modify the
// button? I think the latter is ideal but it leads to an issue of how to store all of the handles.
// Returning a struct would be the approach here, but creating a struct for every widget might get
// annoying. Provide a macro for this if this is to be implemented.
em_result em_add_button(em_ctx*          ctx,
                        em_ui*           ui,
                        em_idx           target_idx,
                        em_handle*       handle,
                        em_button_state* state) {
    em_result res;

    state->pressed     = EM_FALSE;
    em_handler handler = {.callback = em_button_cb, .state = (void*)state, .ref_count = 1};
    em_idx     handler_idx;
    res = em_add_handler(ctx, handler, &ui->resources.handlers, &handler_idx);
    EM_EXPECT(res);

    res = em_create_container(ctx, ui, target_idx, handle);
    EM_EXPECT(res);

    em_res_desc* desc  = &ui->descriptors.data[*handle];
    desc->callback_idx = handler_idx;
    em_handle rect_handle;
    res = em_add_rect(ctx,
                      ui,
                      desc->tree_idx,
                      &rect_handle,
                      (em_rect){.x = 0, .y = 0, .height = 20, .width = 10});
    EM_EXPECT(res);
    em_handle text_handle;
    res = em_add_text(ctx, ui, desc->tree_idx, &text_handle, (em_text){.text = &button_text});

    EM_EXPECT(res);

    return EM_OK;
}

em_result em_add_slider(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle) {
    em_result res;
}

em_result em_add_textbox(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle) {
}

em_result em_add_picture(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle) {
}

em_result em_set_style(em_ui* ui, em_handle handle, em_tag style_tag) {
    if (ui->descriptors.size <= handle) {
        return EM_ERR_OUT_OF_BOUNDS;
    }
    if (ui->resources.styles.size <= style_tag) {
        return EM_ERR_OUT_OF_BOUNDS;
    }
    ui->descriptors.data[handle].style_idx = style_tag;
    return EM_OK;
}

em_result em_set_handler() {
}

// Rarely should the user ever change the primitive of an element but it is a valid operation.
em_result em_set_primitive() {
}
