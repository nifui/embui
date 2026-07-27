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
 *
 * */

#pragma once

#ifndef EM_POOL_H
#define EM_POOL_H

#include "em_ui.h"
#include "em_type.h"

/**
 * @brief A handle to resources, referenced by @ref em_node
 *
 *
 * */
typedef struct em_handle {
    em_idx            tree_idx;     //< Index for the tree. Can be shared
    em_idx            pool_idx;     //< Corresponding pool index specified by @ref type.
    em_idx            style_idx;    //< Index into the style pool. Can be shared.
    em_idx            callback_idx; //< Index into the callback registry. Can be shared.
    em_primitive_type type;         //< Type of the resource. Used to specify the primitive pool.
} em_handle;

/**
 *  @brief Contains a pool of handles.
 *
 * */

typedef struct em_handle_pool {
    size_t     size;
    size_t     capacity;
    em_handle* handles;
} em_handle_pool;

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

typedef EM_VECTOR(em_style, em_style_pool);
typedef EM_VECTOR(em_rect, em_rect_pool);
typedef EM_VECTOR(em_circle, em_circle_pool);
typedef EM_VECTOR(em_line, em_line_pool);

/**
 * @brief Pool containing other pools.
 *
 *
 *
 *
 *
 * */
typedef struct em_resource_pool {
    em_handle_pool handle_pool;
    em_style_pool  style_pool;
    em_rect_pool   rect_pool;
    em_circle_pool circle_pool;
    em_line_pool   line_pool;
} em_resource_pool;

int em_pool_init(em_ctx* ctx, em_resource_pool* pool);

em_result em_add_handle(em_ctx*           ctx,
                        em_resource_pool* resource_pool,
                        em_primitive_type type,
                        em_handle*        handle);

#endif
