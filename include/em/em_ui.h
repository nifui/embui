/**
 * @file em_ui.h
 *
 *
 * */

#pragma once

#ifndef EM_UI_H
#define EM_UI_H

#include "em_math.h"
#include "em_type.h"
#include <stddef.h>
#include "em_tree.h"
#include "em_elements.h"
#include "em_style.h"
#include "em_event.h"

// These are more so for static UIs.
//
// These translate directly to function calls and do not do anything in terms of statically defining
// the UI.
//
// To create reusable widgets, function-like macros are the best approach as it avoids subtree
// operations which can get costly.
//
// Since these can all take variadic children maybe define a helper for users to write variadic
// children widgets aswell?
//
// Macro reflecction is impossible so maybe just leave a template instead or expand into a function
// call with appended behavior.
#define WINDOW(...)
#define TEXT(...)
#define BUTTON(...)
#define RECT(...)

#define VARIADIC_WIDGET()

// Could definitely shorten the name.
// Also tree_idx might be redundant?
typedef struct em_node_desc {
    em_idx tree_idx;      //< Index for the tree. CANNOT be shared.
    em_idx primitive_idx; //< Index into the primitive pool. Can be shared
    em_idx style_idx;     //< Index into the style pool. Can be shared.
    em_idx callback_idx;  //< Index into the callback registry. Can be shared.
} em_node_desc;

typedef struct em_node_descs {
    size_t        size;
    size_t        capacity;
    em_node_desc* data;
} em_node_descs;

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
    em_node_desc descriptors;
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

em_result em_init_ui(em_ctx* ctx, em_ui* ui, em_ui_desc* desc, em_handle* root_handle);
/**
 * @brief Function description
 *
 * @param parameter Description of parameter.
 *
 * @return Return value description
 */
em_result
em_add_prim(em_ctx* ctx, em_ui* ui, em_idx parent_idx, em_idx style_idx, em_primitive_type type);

// For every primitive type define an equivalent function.

em_result em_add_rect();
em_result em_add_text();
em_result em_add_line();
em_result em_add_circle();
em_result em_add_point();

// Define some basic template widgets.
//
em_result em_add_button();
em_result em_add_textbox();
em_result em_add_slider();
em_result em_add_picture();

/**
 * @brief Defines a list of elements that share the same prim.
 *
 * @pre Since this operation takes a tree_idx list, the tree must be updated prior to grab the
 *      requested tree spots.
 * @warnining Since the relationship between handles and trees are strongly linked, one must be
 * partially created and then supplied to create both items.
 *
 * @param shared_count Amount of elements to create that share the same prim.
 * @param[out] dst_handles Array that holds the amount of handles created.
 * @param
 *
 * @retval EM_OK
 */
em_result em_shared_primss(em_ctx*    ctx,
                           em_ui*     ui,
                           em_handle* reference,
                           size_t     shared_count,
                           em_handle* dst_handles,
                           em_idx*    tree_idx);
#endif
