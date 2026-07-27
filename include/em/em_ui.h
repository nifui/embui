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

typedef struct em_ui em_ui;

// clang-format off
typedef enum em_primitive_type { 
    RECT, 
    TEXT, 
    LINE, 
    CIRCLE, 
    POINT 
} em_primitive_type;

// clang-format on

typedef em_recti em_rect;
typedef em_vec2i em_vec2;
typedef em_linei em_line;

typedef struct em_circle {
    em_vec2 center;
    int     radius;
} em_circle;

// 8 or 4 bytes
typedef struct em_text {
    const char** text;
} em_text;

// Avoiding this cause size is unknown.
typedef struct em_polygon {
    em_vec2* points;
} em_polygon;

typedef struct em_primtive {
    em_primitive_type type;
    em_idx            style_idx;

    // At most 32 bytes.
    union {
        em_rect   rect;
        em_vec2   point;
        em_line   line;
        em_circle circle;
        em_text   text;
    };
} em_primitive;

typedef struct em_color {

    uint8_t r, g, b, a;
} em_color;

typedef struct em_rect_style {
    em_vec2 scale;
    int     is_relative;
} em_rect_style;

typedef struct em_text_style {
    em_idx   font_idx;
    em_color text_color;
    em_color highlight_color;
    int      text_size;

} em_text_style;

typedef struct em_line_style {

} em_line_style;

typedef struct em_circle_style {

} em_circle_style;

typedef struct em_point_style {

} em_point_style;

typedef struct em_primitive_pool {
    EM_VECTOR(em_primitive, primitives);
    EM_VECTOR(em_text_style, text_styles);
    EM_VECTOR(em_line_style, line_styles);
    EM_VECTOR(em_point_style, point_styles);
    EM_VECTOR(em_rect_style, rect_styles);
} em_primitive_pool;

// If DMA is not supported/intended, place a function that returns NULL.
// This will indicate to the function calling for a DMA to return that capacity has been reached.
typedef enum em_cmd_type {
    DRAW_RECT = 0,
    DRAW_ROUNDED_RECT,
    DRAW_LINE,
    DRAW_CIRCLE,
    DRAW_ARC,
    DRAW_TEXT,
    DRAW_IMAGE,
    DRAW_SHAPE,
} em_cmd_type;

typedef struct {
    int      rounding;
    em_rect  rect;
    em_color color;
} em_draw_rect;

typedef struct {
    em_vec2i center;
    int      radius;
    em_color color;
} em_draw_circle;

typedef struct {
    em_color color;
    em_vec2i start;
    em_vec2i end;
    int      thickness;
} em_draw_line;

typedef struct {
    em_color color;
    int      text_px;

} em_draw_text;

typedef struct {
    em_color color;
    em_vec2* vertices;
    size_t   vertex_count;

} em_draw_shape;

typedef struct em_cmd {

    em_cmd_type type;

    union {
        em_draw_rect   draw_rect;
        em_draw_circle draw_circle;
        em_draw_line   draw_line;
        em_draw_text   draw_text;
        em_draw_shape  draw_shape;
    };

} em_cmd;

typedef struct em_save_state {
    // Current index the tree is at.
    em_idx tree_index;
    size_t current_depth;
} em_save_state;

// Allow sending commands in batches within a constrained size buffer.
// Avoids dynamic memory allocation and can be used for debugging what's going work.
// Function pointers can also be used instead.
int em_emit_cmd();

int em_ctx_init(em_ctx* ctx, em_allocator allocator);

int em_init_ui(struct em_ui* ui);
#endif
