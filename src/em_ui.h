#pragma once

// Define a macro do ensures that an array has enough capacity to store another element.
// Typical alloc strat should be doubling of memory till the doubling amount meets some threshold.
#ifndef EM_UI_H
#define EM_UI_H
#define VECTOR_TYPE(T)                                                                             \
    typedef struct {                                                                               \
        T*  data;                                                                                  \
        int size;                                                                                  \
        int capacity;                                                                              \
    }##T##_vec
#include "em_real.h"
#include "em_math.h"
// Additional features :
// Native arbitrary polygon rendering.
// In JS and HTML the method for rendering shapes is primarily through svgs or the Canvas API.
// If neeeded support could be added for more shapes.
// If that's the case then a sort of shape struct should be created to merge differnt styling
// methods that might be redundant.
typedef uint32_t em_id;

// Drawing primitives vs UI primtives.
// UI primitives might include multiple drawing primitives to properly draw wheras drawing
// primitives do not.

typedef enum em_primitive_type { RECT, POINT, LINE, TEXT, CIRCLE } em_widget_type;

typedef em_recti em_rect;
typedef em_vec2i em_vec2;
typedef em_linei em_line;

// probably change this to be a drawing primitive
typedef struct em_circle {
    em_vec2 center;
    int     radius;
} em_circle;

typedef struct em_text {
    const char** text;
} em_text;

// Basic UI primitives
// Type acts as a way to index into the respective primitive's styling array.
typedef struct em_primtive {
    em_primitive_type type;
    em_id             style_id;

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
    em_id    font_id;
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

// This could probably be cleaner considering the context field could easilly be confused with the
// regular em_ctx struct.
typedef struct em_allocator {
    void* (*alloc)(size_t size, void* context);
    void* (*realloc)(void* ptr, size_t size, void* context);
    void (*dealloc)(void* ptr, void* context);
    void* context;
} em_allocator;

// Should this be reused?
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

typedef struct {
    int rounding;

} em_draw_rect;

typedef struct {
    int      radius;
    em_vec2i center;
} em_draw_circle;

typedef struct {

} em_draw_line;

typedef struct {
} em_draw_text;

typedef struct em_cmd {
    em_cmd_type type;

    union {
        em_draw_rect   draw_rect;
        em_draw_circle draw_circle;
        em_draw_line   draw_line;
        em_draw_text   draw_text;
    };

} em_cmd;

// This prob isnt needed
typedef struct {
    em_vec2 cursor;
    int     wheel_delta;
    bool    left_down;
    bool    right_down;
    bool    middle_down;
} em_input;

// For an embedded systems specify a starting address that can be used for memory operations. Then
// let the allocator go from there
//
int em_allocator_init(em_allocator* allocator,
                      void* (*alloc)(size_t size, void* context),
                      void* (*realloc)(size_t size, void* context),
                      void (*dealloc)(void* ptr, void* context),
                      void* context);
int em_ctx_init();

em_cmd* em_emit_cmds(em_ctx* ctx) {
}

// By having a next_sibling depth of the tree can be perserved
// First child helps with finding the child.
// Since this is an array that is contiguous elemtn by id search is  a cheap array indexing op.
// More metadata could be added but it might not provide much of a benefit either way.
// By default parent_id 0 refers to root which is created by default when calling the node tree
// creator. Might be best to use some other value instead of referring to own id which could cause a
// cyclic loop
typedef struct em_node {
    em_id id;
    // No parent = refer to own id.
    em_id parent_id;
    // No children = refer to own id.
    em_id first_child;
    // No sibling = refer to own id.
    em_id next_sibling;
} em_node;

// Considering children are allowed how do we determine how to lay them out if the user doesn't
// provide styling? Default styling/layout strategy for children?
//

VECTOR_TYPE(em_rect_style);

// Go with a SoA approach for better cache locality and less memory consumption?
// It might not be better at small elements as the indecision caused by determing which array to
// index might outweight the benefit of a uniform memory layout. It would also allow us to emit
// a non styled tree for debugging when needed.
typedef struct em_node_tree {
    em_rect_style*  rect_styles;
    size_t          rect_style_count;
    em_text_style*  text_style;
    size_t          text_style_count;
    em_point_style* point_styles;
    size_t          point_styles_count;
    em_line_style*  line_styles;
    size_t          line_styles_count;
    em_primitive*   primitives;
    size_t          primitive_count;
    em_node*        nodes;
    size_t          node_count;
} em_node_tree;

//
#define DEFAULT_ARRAY_SIZE 32

int em_node_tree_init(em_ctx* ctx, struct em_node_tree* tree) {
    tree->nodes      = (em_node*)ctx->allocator.alloc(sizeof(em_node) * DEFAULT_ARRAY_SIZE,
                                                      ctx->allocator.context);
    tree->node_count = DEFAULT_ARRAY_SIZE;
    // Maybe default alloc for rect and text as those are most commonly used.
    tree->nodes[0] = (em_node){.id = 0, .parent_id = 0, .first_child = 0, .next_sibling = 0};

    // The other fields should be empty as the user might never use them
}

int em_char_height() {
    return 1;
}

int em_char_width() {
    return 1;
}

bool em_verify_node(em_ctx* ctx) {
    // Verify the node can be actually inserted by checking the ids of the fields to verify it's
    // within the array size
}

void* em_ensure_capac() {
}

// This should be an internal function that the user should not be able to access. If the user were
// able to access this they could put arbitrary nodes with improper ids that lead to improper memory
// accesses.
int em_add_node(em_ctx* ctx, em_node_tree* tree) {

};

#endif
