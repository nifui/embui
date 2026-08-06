/**
 * @file em_ui.h
 *
 * */
#pragma once

#ifndef EM_UI_H
#define EM_UI_H

#include "em_math.h"
#include "em_type.h"
#include "em_tree.h"
#include "em_elements.h"
#include "em_style.h"
#include "em_event.h"

typedef struct em_res_desc {
    em_idx tree_idx;      //< Index for the tree. CANNOT be shared.
    em_idx primitive_idx; //< Index into the primitive pool. Can be shared
    em_idx style_idx;     //< Index into the style pool. Can be shared.
    em_idx callback_idx;  //< Index into the callback registry. Can be shared.
} em_res_desc;

typedef struct em_res_descs {
    size_t       size;
    size_t       capacity;
    em_res_desc* data;
} em_res_descs;

/**
 * @brief em_resources
 */
typedef struct em_resources {
    em_styles     styles;
    em_primitives primitives;
    em_handlers   handlers;
    // Add to each struct a reference counter. At anything above
} em_resources;

/**
 * @brief Descriptor for initializing @ref em_ui.
 *
 * @return Return value description
 */
typedef struct {
    em_node* nodes;
    em_idx   node_capacity;

    em_handle* handles;
    em_idx     handle_capacity;

    em_style* styles;
    em_idx    style_capacity;

    em_primitives* primitives;
    em_idx         primitive_capacity;

    // Issue with how this is done is that when future items get added to this allocation context
    // the strategy of segmenting this grows more complex.
    // Probably should add some sort of free list context struct for better compatibility.
    em_idx* free_idx;
    size_t* free_capacity;

} em_ui_desc;

/**
 * @brief Struct containing em_tree and em_resources.
 */
typedef struct em_ui {
    em_tree      tree;
    em_resources resources;
    em_res_descs descriptors;
} em_ui;

em_result em_pool_init(em_ctx* ctx, em_resources* resources);

int em_ctx_init(em_ctx* ctx, em_allocator allocator);

/**
 * @brief Function for initializing the UI.
 *
 * @param ctx Optional. Memory allocator context. Must be NULL if @p desc is provided.
 * @param ui Pointer to a user initialized UI struct.
 * @param desc Optional. Memory descriptor. Must be NULL if @p ctx is provided.
 * @param[out] root_handle Output parameter that holds the handle to the root node that is
 *             initialized by default.
 *
 * @return Return value description
 */

em_result em_init_ui(em_ctx* ctx, em_ui* ui, em_handle* root_handle, int width, int height);
/**
 * Create a generic primitive element.
 */
em_result em_create_element(em_ctx*      ctx,
                            em_ui*       ui,
                            em_idx       target_idx,
                            em_primitive primitive,
                            em_handle*   handle);

/**
 * Create an empty container element.
 */
em_result em_create_container(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle);

/**
 * The operations below do not reuse/share anything and will create new resources. Avoid using
 * if memory usage is a concern.
 */

/**
 * Add primitive elements.
 */
em_result em_add_rect(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_rect rect);

em_result em_add_text(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_text text);

em_result em_add_line(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_line line);

em_result
em_add_circle(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_circle circle);

em_result
em_add_point(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_point point);

/**
 * @brief Higher-level widgets.
 *
 * Since the field requires a pointer to state, it forces the user to allocate the state
 * themselves. Might not be the biggest issue, but definitely something to note.
 *
 */
typedef struct em_button_state {
    em_bool pressed;
} em_button_state;

em_result
em_add_button(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle, em_button_state* state);

// Made with two closed shapes.
em_result em_add_slider(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle);

// Could be made with just a line as the cursor, a rectangle as the container, and two text
// element or one if ghost text isn't needed.
em_result em_add_textbox(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle);

// Idk how to handle this as this could just be considered a rendering primitive as all you need
// is a rectangle to throw the picture in. If I were to tie it to state, it would be doing
// needless work.
em_result em_add_picture(em_ctx* ctx, em_ui* ui, em_idx target_idx, em_handle* handle);

/**
 * Assign a style tag to an element.
 */
em_result em_set_style(em_ui* ui, em_handle handle, em_tag style_tag);

em_result em_set_handler();

em_result em_set_primitive();

#endif
