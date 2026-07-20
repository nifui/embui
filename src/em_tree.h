#pragma once
#ifndef EM_TREE_H
#define EM_TREE_H
#include <stdint.h>
struct em_ctx;
#ifndef CAPAC_32
#define large uint16_t
#define medium uint8_t
#endif

#ifdef CAPAC_32
#define large uint32_t
#define medium uint16_t
#endif

typedef struct em_widget {
    large  parent;
    large  child_begin;
    medium child_count;
} em_widget;

typedef struct em_widget_tree {
    em_widget *widgets;
    large      widget_count;
} em_widget_array;

em_widget *em_widget_children(em_widget_tree *tree, em_widget *widget);
int       *em_add_child(em_widget_tree *tree, em_widget *parent, em_widget *child);
int       *em_remove_child(em_widget_tree *tree, em_widget *child);
int       *em_prune(em_widget_tree *tree);

#endif
