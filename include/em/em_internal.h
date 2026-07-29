/**
 * @file em_internal.h
 * @brief Defines several internal functions that should not be user callable.
 *
 * Utilizes swap and pop to remove elements making deletions and push operations O(1). Because both
 * tree and handles have a strong connection, order isn't an issue.
 *
 *
 *
 *
 * */

#pragma once

#ifndef EM_INTERNL_H
#define EM_INTERNL_H

#include "em_ui.h"
#include "em_pool.h"
#include "em_type.h"

/**
 * @brief Validates a handles field indices.
 *
 * @invariant Cannot check the tree_idx field as no access. Maybe change in the future.
 * */

em_result em_validate_handle(em_resources* resources, em_handle* handle) {
    TODO("Implement the validate handles function");
    return EM_OK;
}

/**
 * @brief Removes a specified handle via swap and pop.
 *
 * @param parameter Description of parameter.
 *
 * @retval EM_OK
 * @retval EM_ERR_INVALID_INDEX Handle index was invalid
 * @retval EM_ERR_INVALID_HANDLE The handle pointer was NULL.
 *
 *  @invariant Since handles and tree nodes are strongly linked this requires a tree operation
 *             afterward to set the new handle index.
 */
em_result em_remove_handle(em_resources* resources, em_handle* handle, em_idx handle_idx) {
    if (handle_idx == EM_IDX_NULL) {
        return EM_ERR_INVALID_INDEX;
    }
    if (handle == NULL) {
        return EM_ERR_INVALID_HANDLE;
    }
    resources->handles.size--;
    em_handle* last_handle              = &resources->handles.data[resources->handles.size];
    resources->handles.data[handle_idx] = *last_handle;
    return EM_OK;
}

/**
 * @brief Adds a handle with a simple push as the items are guaranteed contiguous.
 *
 */
em_result em_add_handle(em_ctx* ctx, em_resources* resources, em_handle handle) {
    em_result res;
    res = em_reserve(ctx,
                     (void**)&resources->handles.data,
                     &resources->handles.capacity,
                     resources->handles.size + 1,
                     sizeof(em_handle));
    EM_EXPECT(res);

    resources->handles.data[resources->handles.size] = handle;
    resources->handles.size += 1;

    return EM_OK;
}

em_result
em_add_resource(em_ctx* ctx, em_resources* resources, em_primitive_type type, em_idx* res_idx) {
    em_result res;
    res = em_reserve(ctx,
                     (void**)&resources->prims.data,
                     &resources->prims.capacity,
                     resources->prims.size + 1,
                     sizeof(em_prim));
    EM_EXPECT(res);

    resources->prims.data[resources->prims.size] = (em_prim){.type = type};
    resources->prims.size += 1;
    return EM_OK;
}

// Need an equivalent remove_resource call but it requires adding a way of detecting stale handles
// which is basically just a generation counter. Only issue is it'll take up more memory.

#endif
