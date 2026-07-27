#pragma once
#ifndef EM_TREE_H
#define EM_TREE_H

#ifdef AUTOCOMPACT
// If defined, the library will attempt to clean the tree to reduce the free list by either
// condensing or reordering the nodes. Unsure of the strategy. Could make an empty copy of the whole
// thing and fill it up which gives excellent cache locality, but that would waste memory. Throw the
// entries at the front of the list into free list spots to fill them up and shrink size taken up.
// Swap nodes(worse strategy).
//
#endif

#include "em_ui.h"

// 24 bytes
typedef struct em_node {
    // Required
    em_idx parent;
    // Maybe not. Deletions would be O(children) without but with it O(1)
    em_idx prev;
    // Required
    em_idx next;
    // Required
    em_idx first_child;
    // Maybe not. Append would be O(children) without but with it O(1)
    em_idx last_child;
} em_node;

typedef struct em_tree em_tree;

/* lifecycle */
int em_tree_init(em_ctx *ctx, em_tree *tree, size_t size);
int em_tree_free(em_ctx *ctx, em_tree *tree);

/* node operations */
int em_tree_add(em_ctx *ctx, em_tree *tree, em_idx parent);

int em_tree_remove(em_ctx *ctx, em_tree *tree, em_idx node);

int em_tree_extract(em_ctx *ctx, em_tree *tree, em_idx node, em_node *out);

int em_tree_destroy(em_ctx *ctx, em_tree *tree, em_idx node);

int em_tree_unlink(em_tree *tree, em_idx node);

int em_tree_swap(em_tree *tree, em_idx a, em_idx b);

int em_tree_clean(em_tree *tree);

/* subtree operations */
int em_add_subtree(em_ctx *ctx, em_idx dst_idx, em_tree *dst_tree, em_tree *src_tree);
int em_merge_subtree(em_ctx *ctx, em_idx dst_idx, em_tree *dst_tree, em_tree *src_tree);

#endif
