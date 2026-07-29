/**
 * @file em_ui.h
 * @brief Types that are in a pool managed by a central resource pool.
 *
 * Contains pools that are indexed into by the handles. Operations here rely on handles given to the
 * user to be supplied to modify a target element's properties (primitive, styling, callbacks)
 * @defgroup Pool Pool API
 *
 * @invariant Every tree node from em_tree.h has a matching handle.
 * @invariant Handles are returned by reference not index.
 * @invariant Users should never attempt to acquire a handle by index.
 * @invariant When removing a handle, it should always be done in conjugation with it's reference
 *            element.
 * @todo This should be fused with em_ui as otherwise circular dependecy annoyances.
 * */

#pragma once

#ifndef EM_POOL_H
#define EM_POOL_H

#include "em_ui.h"
#include "em_type.h"

/**
 * @brief A handle to resources, referenced by @ref em_node
 * @todo Considering that the prims array and handles array are parallel, maybe fuse the structs?
 *       Only issue with this is that it loads more stuff that might not be important which could
 *       hurt cache locality.
 *
 * */
typedef struct em_handle {
    em_idx tree_idx;     //< Index for the tree. Can be shared
    em_idx prim_idx;     //< Corresponding pool index specified by @ref type.
    em_idx style_idx;    //< Index into the style pool. Can be shared.
    em_idx callback_idx; //< Index into the callback registry. Can be shared.
} em_handle;

/**
 *  @brief Contains a pool of handles.
 *
 * */

// approach 1: switch on type and index.
// more packed memory
// worse cache locality
// approach 2: index and switch on type and then interpret field.
// very cache friendly as contiguous
// wasted memory
// adding = no branch prediction as the type are in the same array.
// avoids having to leap from an array to another and pointer chasing.
// less of a hassle to write code that only cares about adding and creating.
//
typedef struct em_handles {
    size_t     size;
    size_t     capacity;
    em_handle* data;
} em_handles;

/**
 * @brief Styling for elements.
 *
 * @invariant Certain qualities cannot be used on certain primitives.
 * @invariant If a default stlye does not exist before an element is added, one will be
 *            automatically created and assigned to the element.
 *
 *
 * */
typedef struct em_style {
} em_style;

typedef EM_VECTOR(em_prim, em_prims);
typedef EM_VECTOR(em_style, em_styles);

/**
 * @brief Pool containing other pools.
 *
 *
 *
 *
 *
 * */
typedef struct em_resources {
    em_handles handles;
    em_styles  styles;
    em_prims   prims;

} em_resources;

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

#endif
