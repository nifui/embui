#pragma once

#ifndef EM_POOL_H
#define EM_POOL_H

#define POOL_SIZE 32
#include "em_ui.h"

typedef struct em_handle {
    em_idx            pool_index;
    em_primitive_type type;
    em_idx            style_index;
} em_handle;

typedef struct em_handle_pool {
    size_t     size;
    size_t     capacity;
    em_handle* handles;
} em_handle_pool;

typedef struct em_style {
} em_style;

// The handle must maintain a stable style index. If reordering were to happen, styles should not be
// reordered.
typedef EM_VECTOR(em_style, em_style_pool);
typedef EM_VECTOR(em_rect, em_rect_pool);
typedef EM_VECTOR(em_circle, em_circle_pool);
typedef EM_VECTOR(em_line, em_line_pool);

// The traverser looks up the handle within the handle pool. Then it looks at the type and
// determines the pool to search for. Then it indexes into said pool. For shared styling the index
// might be shared across two differnt elements. Help's with batching items.
typedef struct em_resource_pool {
    // jump to the specific pool via a pointer.
    em_handle_pool  handle_pool;
    em_style_pool*  style_pool;
    em_rect_pool*   rect_pool;
    em_circle_pool* circle_pool;
    em_line_pool*   line_pool;
} em_resource_pool;

int em_pool_init(em_ctx* ctx, em_resource_pool* pool) {
}

#endif
