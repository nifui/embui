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

typedef struct em_resource_ref {
    em_idx tree_idx;     //< Index for the tree.
    em_idx prim_idx;     //< Corresponding pool index specified by @ref type.
    em_idx style_idx;    //< Index into the style pool. Can be shared.
    em_idx callback_idx; //< Index into the callback registry. Can be shared.
} em_resource_reference;

typedef struct em_resource_references {
    size_t                 size;
    size_t                 capacity;
    em_resource_reference* data;
} em_resource_references;

/**
 * @brief Styling for elements.
 *
 * @invariant Certain qualities cannot be used on certain primitives.
 * @invariant If a default stlye does not exist before an element is added, one will be
 *            automatically created and assigned to the element.
 *
 *
 * */

// Allow a layout specifier.
// Layout's are applied to the children so a state variable needs to be kept for specifically
// calculating children layout.
typedef enum em_direction {
    HORIZONTAL,
    VERTICAL,
} em_direction;

typedef struct em_layout {
    em_direction direction;
    char*        constraints;
    char         margin;
    char         padding;

} em_layout;

/**
 * @brief Styling for primitives.
 *
 * @todo Add animation support.
 * @note To calculate the animation frames needed, collect the referenced styles.
 */

typedef struct em_style {
    em_layout layout;
    em_color  text_color;
    em_color  bg_color;
    em_color  border_color;
    char      rounding;
    em_idx    next_frame;

} em_style;

#define EM_STYLE_DEFAULT (em_style){.layout = 0}

typedef EM_VECTOR(em_prim, em_prims);
typedef EM_VECTOR(em_style, em_styles);

/**
 * @brief Opaque type to em_resources
 */

// If we avoid using a freelist we have to do a swap and pop strategy which leads to resource_refs
// point to invalid resources. With a free list approach a generation counter isn't even needed as
// all you need is to check if an index is in the free list. However this defauls to a linear scan
// which can be fast assuming small free list. A generation counter in the handle would solve this
// at the cost of some additional memory. For each group of related structures a generation counter
// would be needed. Since each array can have stuff removed without affecting the other array each
// array has to have their own generation counter. The smart move would be to shift away from using
// seperate systems into a unified node but it leads to worse cache locality.
//
//
//
// We could make each resource store an array of the resource_refs handles that can allow us to
// invalidate the resource without a search. This comes with huge memory costs.
//
// Reference counting would only solve the issue of shared memory leaking. Without reference
// counting we don't know if an item is still in use by other objects and so we leave it alone
// instead of removing it leading to orphaned items. A and B share C. A is removed. B is then
// removed. Nothing references C but it doesn't know so it stays alive. With reference counting on C
// it now knows that nothing relies on it so it can be freed. Relies on a manual call to a
// cleanup function however to be valid.
//
// A generation + free list solves the problem of invalid references that might occur with direct
// resource removal. A and B share C. We remove C but A and B still reference C and don't know it's
// been removed/invalid. With a generation counter this is solved.
//
// Guaranteed traits:
// For every resource_reference there is only one handle for it. For a deletion operation on the
// resource_ref via a handle, the handle is invalidated and the resource ref removed. Then via
// reference counting we can determine whether to open a slot up for reuse. (Requires a free list
// for each shared resource + references field in each resource). References can just be a single
// byte.
//
typedef struct em_resources {
    em_styles styles;
    em_prims  prims;
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

    em_prim* prims;
    em_idx   prim_capacity;
} em_ui_desc;

/**
 * @brief Struct containing em_tree and em_resources.
 */
typedef struct em_ui {
    em_tree      tree;
    em_resources resources;
} em_ui;

em_result em_pool_init(em_ctx* ctx, em_resources* resources);

/**
 *
 *
 *
 *
 * */
em_result em_modify_style(em_resources* resources, em_idx style_idx, em_style style);

em_result em_add_style(em_ctx* ctx, em_resources* resources, em_style* style, em_idx* dst_idx);

em_result em_change_style(em_ctx* ctx, em_resources* resources, em_idx target);

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

/**
 * @brief Defines a list of elements that share the same prim.
 *
 * @pre Since this operation takes a tree_idx list, the tree must be updated prior to grab the
 *      requested tree spots.
 * @warnining Sinec the relationship between handles and trees are strongly linked, one must be
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
