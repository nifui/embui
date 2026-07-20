#pragma once
#ifndef EM_UI_H
#define EM_UI_H
#include "em_math.h"
typedef em_recti em_rect;
typedef em_vec2i em_vec2;
typedef em_linei em_line;
typedef struct em_circle {
    em_vec2 center;
    int     radius;
} em_circle;

typedef struct em_text {
    const char **text;
    int          font_id;
} em_text;

typedef struct em_color {
    uint8_t r, g, b, a;
} em_color;

typedef struct em_rect_style {
    em_vec2 scale;
    int     is_relative;
} em_rect_style;

typedef struct em_text_style {
    int      font_id;
    em_color text_color;
    int      text_size;
} em_text_style;

typedef struct em_allocator {
    void *(*alloc)(size_t size, void *context);
    void *(*realloc)(size_t size, void *context);
    void (*dealloc)(void *ptr, void *context);
    void *context;
} em_allocator;

typedef struct em_ctx {
    em_allocator allocator;
} em_ctx;

typedef enum em_cmd_type {
    DRAW_RECT = 0,
    DRAW_ROUNDED_RECT,
    DRAW_LINE,
    DRAW_CIRCLE,
    DRAW_ARC,
    DRAW_TEXT,
    DRAW_IMAGE
} em_cmd_type;

typedef struct em_cmd {
    em_cmd_type type;
    union {
        int draw_rect;
        int draw_a;
        int draw_b;
        int draw_c;
        int draw_d;
        int draw_e;
        int draw_f;
    };
} em_cmd;

typedef struct {
    em_vec2 cursor;
    int     wheel_delta;
    bool    left_down;
    bool    right_down;
    bool    middle_down;
} em_input;

int em_allocator_init(
    em_allocator *allocator,
    void *(*alloc)(size_t size, void *context),
    void *(*realloc)(size_t size, void *context),
    void (*dealloc)(void *ptr, void *context),
    void *context
);

#endif
