#pragma once
#include <stdlib.h>
#include <stdint.h>

// By default any element that is not visible will not issue a draw command. Updating the element's
// state is the responsibility of the user.
#define CONTAINS_FLAG(flags, flag)
#define t_max(x, y) (((x) > (y)) ? (x) : (y))
#define t_min(x, y) (((x) < (y)) ? (x) : (y))

// Replace this with a union later to support other formats.
typedef struct t_color {
    uint8_t r, g, b, a;
} t_color;

typedef enum t_node_type { RECT = 0, TEXT } t_node_type;

// Basically everything can be made with a combination of these two primitives. There might be some
// more exotic primitives that idk about
typedef struct t_rect {
    int x, y, width, height;
} t_rect;

typedef struct t_text {
    const char* text;
    // reference to a font specified as the id for easier access.
    int font_id;
} t_text;

typedef struct t_node {
    t_node_type type;

    union {
        t_rect rect;
        t_text text;
    };
} t_node;

typedef struct t_vec2 {
    int x, y;
} t_vec2;

// Bitflags for layout.
enum {
    CENTERED = (1 << 0),
    LEFT     = (1 << 1),
    RIGHT    = (1 << 2),
    BOTTOM   = (1 << 3),
    UP       = (1 << 4),
    PERCENT  = (1 << 5),
};

typedef struct t_depth_stack {
    uint8_t* depths;
    uint16_t depth_count;
} t_depth_stack;

typedef struct t_ctx {
    void* (*malloc)(size_t size);
    void* (*realloc)(void* ptr, size_t size);
    t_depth_stack depth_stack;

#ifdef USE_QUADTREE
    t_quadtree quadtree;
#endif
#ifdef USE_LOCALITY_CACHE
    t_locality_cache cache;
#endif

} t_ctx; // Add conditional compilation here to avoid storing fields that the user might never use.

typedef struct t_rect_style {
    t_vec2 scale;
    // Is this element dependent on any other rects to determine it's own dimensions?
    int is_relative;
#ifdef USE_ROUNDED
    // values less than 1 indicates no rounded corners.
    int radius_px;
#endif
#ifdef USE_BORDER
    int          border_px;
    t_colt_color color;
#endif
} t_rect_style;

typedef struct t_text_style {
    void*   font_id;
    t_color text_color;
    int     text_size;
} t_text_style;

typedef struct t_element_base {
    int    id;
    t_rect rect;
} t_element_base;

// Each command should emit a target id to help with managing what the command should act on.
// Allows the user to keep track themselves of the elements if needed.
typedef enum t_cmd_type {
    DRAW_RECT = 0,
    DRAW_TEXT,
    DRAW_IMG,
    DRAW_ANIMATION,
} t_cmd_type;

typedef struct t_cmd_base {
    t_cmd_type type;
    int        ref_id;
} t_cmd_base;

typedef struct t_draw_rect {
    t_cmd_base base;
    t_rect     rect;
    t_color    color;
} t_draw_rect;

typedef struct t_draw_text {
    t_cmd_base base;
    t_color    color;
} t_draw_text;

// This most likely isn't needed because the user handles stuff like uploading the image to the GPU
// to be drawn so this is redundant;
typedef struct t_draw_img {
    t_cmd_base base;
    t_rect     rect;
} t_draw_img;

typedef struct t_cmd {
    union {
        t_cmd_type  type;
        int         ref_id;
        t_draw_rect draw_rect;
        t_draw_text draw_text;
        t_draw_img  draw_img;
    };
} t_cmd;

// Tells the renderer the depth of the elements/z-index to allow proper clipping of hidden elements.
// If we detect that an element is outside of its parent's area via we can emit a check for nodes
// that are on the same depth level as the parent. This allows elements that might be split across
// two or more elements. This is probably more trouble than its worth to implement.
// Maybe also maintain a quad tree as depth of elements do matter for propogating events of a
// clicked element.
// Allows easier tracing of elements that a cursor might be in.
// Could be controlled further with the use of a bisection count.
#ifdef USE_QUADTREE
typedef struct t_quadtree {

} t_quadtree;

typedef struct t_quadnode {

} t_quadnode;

void t_update_quadtree(t_ctx* ctx, t_quadtree* tree) {
}
#endif
/// Stores around 8 elements that are near both the element on the same depth levels and elements
/// near said depth level aswell.
#ifdef USE_LOCALITY_CACHE
typedef struct t_locality_cache {

} t_locality_cache;
#endif

// This might be not ideal considering it forces the user into using this specific format which
// might be less memory efficient and limited in extensibility. Also the scroll and cursor could be
// changed to emit deltas instead as thats the typical format emitted by most windowing libraries.
// This also doesn't account for the fact that the UI  might be toggleable so these events shouldn't
// even be sent in that case.
typedef enum t_mouse_event {
    HOLD,
    LCLK,
    RCLK,
    MCLK,
    SCRLU,
    SCRLD,
    CURSOR,
} t_mouse_event;

// Vector addition.
t_vec2 t_vec2_add(t_vec2 a, t_vec2 b);
// Vector subtraction
t_vec2 t_vec2_sub(t_vec2 a, t_vec2 b);

/*
 * Rounding strategy: ?
 * */
// Scalar vector multiply.
t_vec2 t_vec2_smul(t_vec2 a, int s);
// Scalar vector divison.
t_vec2 t_vec2_sdiv(t_vec2 a, int s);

// Calculates the rect that intersects both rectangles.
t_rect t_rects_overlap(t_rect* a, t_rect* b);
// Calculates the rect that has both rects within it.
t_rect t_rects_bbox(t_rect* a, t_rect* b);
// Splits the provided rectangles into the requested quantities.
// Fractiona part strategy: ?
t_rect* t_rect_split(t_rect* rect, int count);
// Checks if a rectangle contains a vec2.
// Does not handle the ccase where the vec2 is on the edge of the rectangle.
bool t_rect_contain_vec2(t_rect* a, t_vec2* b);
// Initialzies the UI context.
void t_init_ctx(t_ctx* ctx, void* (*malloc)(size_t size), void* (*realloc)(void* ptr, size_t size));
// Takes the node tree from t_ctx and the depth_stack to emit a list of commands for drawing the UI.
t_cmd* t_emit_cmds(t_ctx*);
