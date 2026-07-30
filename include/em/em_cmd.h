#pragma once
#ifndef EM_CMD_H
#define EM_CMD_H

#include "em_ui.h"
#include "em_type.h"

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

#define EM_CALL

// Prevent switching between different command methods.
#ifdef EM_EMIT
int em_emit_cmd();

#elif defined(EM_FIXED)
int em_fixed_cmd();

#elif defined(EM_CALL)
// Requires drawer to have valid functions for the specified primitives.
#include "em_draw.h"
int em_call_cmd(em_ctx* ctx, em_ui* ui, em_drawer* drawer);

#endif
#endif
