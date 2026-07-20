#pragma once
#ifndef EM_TREE_H
#define EM_TREE_H
#include <stdint.h>
#include <cstddef>
struct em_ctx;

typedef struct em_widget {
    uint32_t parent;
    uint32_t child_begin;
    uint16_t child_count;
} em_widget;

typedef struct em_widget_tree {
    em_widget* widgets;
    uint32_t   widget_count;
} em_widget_array;

em_widget* em_widget_children(em_widget_tree* tree, em_widget* widget);

int em_init_tree(em_widget_tree* tree) {
    tree->widgets      = NULL;
    tree->widget_count = 0;
}

int* em_add_child(em_ctx* ctx, em_widget_tree* tree, em_widget* parent, em_widget* child) {

};
int* em_remove_child(em_widget_tree* tree, em_widget* child) {};

int* em_prune(em_widget_tree* tree) {
    return 0;
}

#endif
