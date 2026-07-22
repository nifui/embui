#pragma once

#ifndef ALLOC_IMPL
#define ALLOC_IMPL
#include "em_alloc.h"
#endif

// Define a macro do ensures that an array has enough capacity to store another element.
// Typical alloc strat should be doubling of memory till the doubling amount meets some threshold.
#ifndef EM_UI_H
#define EM_UI_H
#define EM_VECTOR(type, name) \
    struct {                  \
        type  *data;          \
        size_t size;          \
        size_t capacity;      \
    } name
#include "em_real.h"
#include "em_math.h"
// Additional features :
// Native arbitrary polygon rendering.
// In JS and HTML the method for rendering shapes is primarily through svgs or the Canvas API.
// If neeeded support could be added for more shapes.
// If that's the case then a sort of shape struct should be created to merge differnt styling
// methods that might be redundant.
typedef uint32_t em_index;
#define EM_NODE_NULL UINT32_MAX
#define STACK_SIZE 32

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
    const char **text;
} em_text;

// Basic UI primitives
// Type acts as a way to index into the respective primitive's styling array.
typedef struct em_primtive {
    em_primitive_type type;
    em_index          style_index;

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
    em_index font_index;
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
    void *(*alloc)(size_t size, void *context);
    void *(*realloc)(void *ptr, size_t size, void *context);
    void (*dealloc)(void *ptr, void *context);
    void *context;
} em_allocator;

// If an implementation is not provided it'll use a default allocator suited for the OS that is
// being targetted.

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
int em_allocator_init(
    em_allocator *allocator,
    void *(*alloc)(size_t size, void *context),
    void *(*realloc)(size_t size, void *context),
    void (*dealloc)(void *ptr, void *context),
    void *context
);
int em_ctx_init();

// By having a next_sibling depth of the tree can be perserved
// First child helps with finding the child.
// Since this is an array that is contiguous elemtn by id search is  a cheap array indexing op.
// More metadata could be added but it might not provide much of a benefit either way.
// By default parent_id 0 refers to root which is created by default when calling the node tree
// creator. Might be best to use some other value instead of referring to own id which could cause a
// cyclic loop

typedef struct em_layout {

} em_layout;

typedef struct em_style {

} em_style;

// Instead of having the style contain object specific properties, maybe use flags?
// For better efficiency a height field could be included but would add an extra field to fill.
typedef struct em_node {
    em_index parent;

    em_index prev;
    em_index next;

    em_index first_child;
    em_index last_child;
} em_node;

// Considering children are allowed how do we determine how to lay them out if the user doesn't
// provide styling? Default styling/layout strategy for children?
//

typedef struct em_node_tree {
    EM_VECTOR(em_node, nodes);
    EM_VECTOR(em_index, free_list);
} em_node_tree;

// Allows styling reuse but at the cost of an extra layer of indecision due to having to jump to
// another array to fetcth the appropiate style.
// ALong with that the branching is relatively  expensive compared to just a simple pointer.
//
typedef struct em_primitive_pool {
    EM_VECTOR(em_primitive, primitives);
    EM_VECTOR(em_text_style, text_styles);
    EM_VECTOR(em_line_style, line_styles);
    EM_VECTOR(em_point_style, point_styles);
    EM_VECTOR(em_rect_style, rect_styles);
} em_primitive_pool;

//
#define DEFAULT_ARRAY_SIZE 32

int em_node_tree_init(em_ctx *ctx, struct em_node_tree *tree) {
    tree->nodes.data =
        (em_node *)
            ctx->allocator.alloc(sizeof(em_node) * DEFAULT_ARRAY_SIZE, ctx->allocator.context);
    tree->nodes.size = DEFAULT_ARRAY_SIZE;
    // Maybe default alloc for rect and text as those are most commonly used.

    // The other fields should be non allocated as the user might never use them
}

//-1 = Failed to allocate memory.
//
int em_tree_add_node(em_ctx *ctx, em_node_tree *tree, em_index parent_index) {
    em_allocator *allocator = &ctx->allocator;
    if (tree->nodes.size == tree->nodes.capacity) {
        size_t new_capacity = tree->nodes.capacity * 2;

        void *res =
            allocator
                ->realloc(tree->nodes.data, new_capacity * sizeof(em_node), allocator->context);

        if (!res)
            return -1;

        tree->nodes.data     = (em_node *)res;
        tree->nodes.capacity = new_capacity;
    }
    em_index free_index;
    if (tree->free_list.size > 0) {
        tree->free_list.size--;
        free_index = tree->free_list.data[tree->free_list.size];
    } else {
        free_index = tree->nodes.size;
        tree->nodes.size += 1;
    }
    em_node *parent_node = &tree->nodes.data[parent_index];
    // Does not handle an empty child list.
    if (parent_node->last_child == EM_NODE_NULL) {
        //
    }
    em_node *last_child_node     = &tree->nodes.data[parent_node->last_child];
    last_child_node->next        = free_index;
    tree->nodes.data[free_index] = (em_node){
        .parent      = parent_index,
        .prev        = parent_node->last_child,
        .next        = EM_NODE_NULL,
        .first_child = EM_NODE_NULL,
        .last_child  = EM_NODE_NULL,
    };
    parent_node->last_child = free_index;
    return 0;
    // Since we don't know the siblings of this node we have to search for it which in the best case
    // is constant time assuming the parent does not have any children or could be O(n) where n is
    // the amount of siblings the parent has. This could be improved with an id to the last child
    // but that increases the amount of metadata that has to be tracked.
};

typedef enum em_remove_strat {
    DISCARD_CHILDREN  = 0,
    REASSIGN_CHILDREN = 1,
    // Might not be worth implementing as the operation of removing all the childrens can grow
    // expensive.
    RETURN_CHILDREN = 2,
} em_remove_strat;

//-1 = Reading uninit memory.
//-2 = Allocation failed.

int em_tree_remove_node(
    em_ctx *ctx, em_node_tree *tree, em_index node_index, em_remove_strat strat
) {
    // Capacity doubling strategy but it could be replaced with a better strategy.
    if (node_index >= tree->nodes.size) {
        return -1;
    }
    if (tree->free_list.size == tree->free_list.capacity) {
        size_t new_capacity = tree->nodes.capacity * 2;

        void *res = ctx->allocator.realloc(
            tree->free_list.data, new_capacity * sizeof(em_node), ctx->allocator.context
        );
        if (!res) {
            return -2;
        }
        tree->nodes.data = (em_node *)res;
    }
    em_node *node         = &tree->nodes.data[node_index];
    em_index parent_index = node->parent;
    // if this is the last or first child of the parent then we should set it to something else.
    em_node *parent_node = &tree->nodes.data[parent_index];

    // Handles if it's the only node, first or last node. Missing a case I think.
    if (parent_node->first_child == node_index && parent_node->last_child == node_index) {
        parent_node->first_child = EM_NODE_NULL;
        parent_node->last_child  = EM_NODE_NULL;
    } else if (parent_node->first_child == node_index) {
        // Shift the children down to fill the gap and avoid orphaned nodes.
    } else if (parent_node->last_child == node_index) {
        parent_node->last_child = EM_NODE_NULL;
    }
    // Handles when it's in the middle.
    if (node->next != EM_NODE_NULL && node->prev != EM_NODE_NULL) {
        em_node next_node = tree->nodes.data[node->next];
        em_node prev_node = tree->nodes.data[node->prev];
        next_node.prev    = node->prev;
        prev_node.next    = node->next;
    }
    if (node->first_child != EM_NODE_NULL) {
        switch (strat) {
        case DISCARD_CHILDREN:

            break;
        case REASSIGN_CHILDREN:
            // Relinks the children of the removed to the children of the parent of the removed.
            // Is put at the end.
            break;
        case RETURN_CHILDREN:

            break;
        }
    }
    // Push the node index into the free list for reuse later.
    tree->free_list.data[tree->free_list.size] = node_index;
    tree->free_list.size += 1;
    return 0;
}

// Given the start of the child list this will remove all children associated to the node.
int em_remove_children(em_ctx *ctx, em_node_tree *tree, em_index start_index) {
    // Allocate space to the free_list if needed.
    // Holds the nodes to free. Since this is temporary that we can guarantee will return back, its
    // fine to allocate over.

    size_t    stack_capacity = STACK_SIZE;
    em_index *free_stack =
        (em_index *)ctx->allocator.alloc(stack_capacity * sizeof(em_index), ctx->allocator.context);
    free_stack[0]     = start_index;
    size_t stack_size = 1;

    while (stack_size >= 1) {
        em_index index     = free_stack[stack_size];
        em_node  prev_node = tree->nodes.data[index];
        while (prev_node.next != NULL)
        // If there are no siblings and no children then break as we have removed all the needed
        // children.
    }
}

// Maybe compose the above methods out of these to make the logic easier to follow.
int unlink_node() {}
int destroy_subtree() {}
int free_node() {}
int reassign_children() {}

int em_char_height() {
    return 1;
}

int em_char_width() {
    return 1;
}

em_cmd *em_emit_cmds(em_ctx *ctx, em_node_tree *tree, em_primitive_pool *pool) {
    size_t  index = 0;
    em_cmd *cmds =
        (em_cmd *)
            ctx->allocator.alloc((tree->nodes.capacity * sizeof(em_cmd)), ctx->allocator.context);
    size_t cmds_size = 0;
    // This should be improved instead of hardcoding an arbitrary size. To conserve space for this
    // aswell, we can specify ranges.
    // Since each node cotnains the indices for the first and last child we can use that to specify
    // a range for a more compact stack.
    em_index *index_stack =
        (em_index *)ctx->allocator.alloc((STACK_SIZE * sizeof(em_index)), ctx->allocator.context);
    size_t index_stack_size = 1;
    // Insert the root node as the first index to search.
    index_stack[0] = 0;
    while (index_stack_size >= 1) {
        em_node node = tree->nodes.data[index];
        em_cmd  cmd;
        // In each case layout has to be calculated regardless so the slowdown from this is probably
        // neglible.
        switch (pool->primitives.data[index].type) {
        case RECT:
            cmd = {.type = DRAW_RECT, .draw_rect = {0}};
            break;
        case POINT:
            break;
        case LINE:
            break;
        case TEXT:
            break;
        case CIRCLE:
            break;
        }
        cmds[cmds_size] = cmd;
        cmds_size += 1;
        if (node.first_child != EM_NODE_NULL) {
        }
    }
}

#endif
