#include <em/em_tree.h>

#ifndef EM_MEMCPY
#include <string.h>
#define EM_MEMCPY memcpy
#endif
typedef struct em_tree {
    EM_VECTOR(em_node, nodes);
    EM_VECTOR(em_idx, free_list);
} em_tree;

// ONLY FOR USE IN TREE MANAGEMENT.
static em_idx em_alloc_idx(em_ctx* ctx, em_tree* tree) {
    if (tree->free_list.size > 0) {
        tree->free_list.size -= 1;
        return tree->free_list.data[tree->free_list.size];
    }
    if (tree->nodes.size >= tree->nodes.capacity) {
        return (em_idx)tree->nodes.size;
    } else {
    }
}

static void em_node_link(em_tree* tree) {
}

static void em_node_unlink(em_tree* tree) {
}

static void em_promote() {
}

em_result em_tree_add(em_ctx* ctx, em_tree* tree, em_idx parent, em_idx* handle_idx) {
    if (!tree) {
        return EM_ERR_MISSING_TREE;
    }
    if (parent == EM_NODE_NULL) {
        return EM_ERR_INVALID_ARGUMENT;
    }
    if (parent >= (em_idx)tree->nodes.size) {
        return EM_ERR_OUT_OF_BOUNDS;
    }
    em_idx new_idx;
    return new_idx;
}

em_result em_tree_remove(em_ctx* ctx, em_tree* tree, em_idx target) {
}

em_result em_node_extract(em_ctx* ctx, em_tree* tree, em_idx node, em_node* out) {
}

em_result em_node_destroy(em_ctx* ctx, em_tree* tree, em_idx node) {
}

em_result em_node_swap(em_tree* tree, em_idx a, em_idx b) {
}

em_result em_tree_clean(em_tree* tree) {
}

em_result em_add_subtree(em_ctx* ctx, em_idx dst_idx, em_tree* dst_tree, em_tree* src_tree) {
}
