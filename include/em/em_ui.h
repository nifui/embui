// Fixes :
// Make realloc optional and use a combination of alloc and free to resize.
// Allow fixed storage instead of only DMA.
// Allow selecting different storage implementations for the callback registry
// Hashmap but probably not a BST.
// Assumptions:
// The user actually knows how to properly model their UI instead of nesting 100 nodes or something.
// 10 or more nested nodes is probably the sweet spot and is deterministic enough.

#pragma once

#ifndef EM_UI_H
#define EM_UI_H
#include "em_math.h"
#include "em_type.h"
#include <stddef.h>
// Drawing primitives vs UI primtives.
// UI primitives might include multiple drawing primitives to properly draw wheras drawing
// primitives do not.

typedef struct em_ui em_ui;

typedef enum em_primitive_type { RECT, POINT, LINE, TEXT, CIRCLE } em_primitive_type;
// 16 bytes

typedef em_recti em_rect;
// 8 bytes
typedef em_vec2i em_vec2;
// 16 bytes
typedef em_linei em_line;

// 12 bytes
typedef struct em_circle {
    em_vec2 center;
    int     radius;
} em_circle;

// 8 or 4 bytes
typedef struct em_text {
    const char **text;
} em_text;

// Avoiding this cause size is unknown.
typedef struct em_polygon {
    em_vec2 *points;
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
    em_vec2 *vertices;
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

// Maybe add an option to allow sending commands in a fixed size to avoid dynamic memory
// allocations. Would require a state struct to record where the processor currently is in the tree.
// For this we keep only the next index to process based off of the sibling or child.
// The priority is first processing the child as we wanna go down as deep as possible in a single
// branch. Then we prioritize prcoessing the siblings. Once the siblings have been processed we can
// go back up to the parent ndoe and then process it's siblings. The process is repeated again until
// we have basically finished processing the whole thing. This works on the assumption that we
// attempt to target the first child first of each parent node. If we want to perserve order for
// drawing we might aswell throw in a depth counter. This basically leaves the reordering and hiding
// stuff up to the user to optimize. However this assumes the user is drawing a crap ton of items
// which probably isn't happening on an embededd system so the performance diff from this is
// neglible especially comapred to dealing with DMA.
// Final strategy = pre order DFS with a depth field and final processing at the end done by the
// user by re ordering the depths.
int em_emit_cmd() {}

int em_ctx_init(em_ctx *ctx, em_allocator allocator) {
    ctx->allocator = allocator;
    return 0;
};

int em_init_ui(struct em_ui *ui);
#endif
