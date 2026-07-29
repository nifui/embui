// Fixes :
// Make realloc optional and use a combination of alloc and free to resize.
// Allow fixed storage instead of only DMA.
// Allow selecting different storage implementations for the callback registry
// Hashmap but probably not a BST.
// Assumptions:
// The user actually knows how to properly model their UI instead of nesting 100 nodes or something.
// 10 or more nested nodes is probably the sweet spot and is deterministic enough.

/**
 * @file em_ui.h
 *
 *
 *
 *
 *
 *
 *
 * */

#pragma once

#ifndef EM_UI_H
#define EM_UI_H

#include "em_math.h"
#include "em_type.h"
#include <stddef.h>
typedef struct em_ui     em_ui;
typedef struct em_handle em_handle;

typedef enum em_primitive_type {
    RECT,
    TEXT,
    LINE,
    CIRCLE,
} em_primitive_type;

#ifdef USE_INT16
// 16 bytes
typedef em_recti16 em_rect;
// 16 byte
typedef em_linei16 em_line;
// 8 bytes
typedef em_vec2i16 em_vec2;

#else
// 8 bytes
typedef em_recti em_rect;
// 8 bytes
typedef em_linei em_line;
// 4 bytes
typedef em_vec2i em_vec2;

#endif
// 4 butes
typedef em_vec2 em_point;

// 12  or 6 bytes
typedef struct em_circle {
    em_vec2 center;
    int     radius;
} em_circle;

// 8 or 4 bytes
typedef struct em_text {
    const char** text;
} em_text;

typedef struct {
    em_primitive_type type;

    union {
        em_rect   r;
        em_line   l;
        em_circle c;
        em_point  p;
        em_text   t;
    };
} em_prim;

typedef struct em_color {
    uint8_t r, g, b, a;
} em_color;

// Creates a new element

int em_ctx_init(em_ctx* ctx, em_allocator allocator);

int em_init_ui(struct em_ui* ui);

/**
 * @brief Function description
 *
 * @param parameter Description of parameter.
 *
 * @return Return value description
 */
em_result em_add_prim(em_ctx* ctx, em_ui* ui, em_idx parent_idx, em_primitive_type type);

/**
 * @brief Defines a list of elements that share the same prim.
 *
 *
 * @param shared_count Amount of elements to create that share the same prim.
 * @param[out] dst_handles Array that holds the amount of handles created.
 * @param
 *
 * @retval EM_OK
 */

em_result em_shared_prims(size_t shared_count, em_handle* dst_handles);

#endif
