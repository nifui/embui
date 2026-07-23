#include "em_tree.h"
#include "em_ui.h"

// Things to fix
// bad error handling with magic number error codes.

typedef struct em_tree {
    EM_VECTOR(em_node, nodes);
    EM_VECTOR(em_idx, free_list);
} em_tree;

/*
 * Internal Tools
 **/

static inline size_t em_max_size(size_t a, size_t b) {
    return a > b ? a : b;
}

static int
em_reserve(em_ctx* ctx, void** data, size_t* capacity, size_t min_capacity, size_t elem_size) {
    if (*capacity >= min_capacity)
        return 0;

    size_t new_capacity = *capacity ? *capacity : 1;
    while (new_capacity < min_capacity)
        new_capacity *= 2;

    void* ptr = ctx->allocator.realloc(*data, new_capacity * elem_size, ctx->allocator.context);

    if (!ptr)
        return -1;

    *data     = ptr;
    *capacity = new_capacity;

    return 0;
}

static em_idx em_alloc_idx(em_ctx* ctx, em_tree* tree) {
    // Prefer reuse from free_list
    if (tree->free_list.size > 0) {
        tree->free_list.size -= 1;
        return tree->free_list.data[tree->free_list.size];
    }

    // Otherwise grow nodes.size by 1
    // Ensure nodes capacity for the new element.
    if (em_reserve(ctx,
                   (void**)&tree->nodes.data,
                   &tree->nodes.capacity,
                   tree->nodes.size + 1,
                   sizeof(em_node)) != 0) {
        return (em_idx)EM_NODE_NULL; // caller must treat as failure
    }

    em_idx idx = (em_idx)tree->nodes.size;
    tree->nodes.size += 1;
    return idx;
}

static int em_free_idx(em_ctx* ctx, em_tree* tree, em_idx idx) {
    if (idx < 0)
        return -1;

    if (tree->free_list.size == tree->free_list.capacity) {
        int rc = em_reserve(ctx,
                            (void**)&tree->free_list.data,
                            &tree->free_list.capacity,
                            tree->free_list.size + 1,
                            sizeof(em_idx));
        if (rc != 0)
            return rc;
    }

    tree->free_list.data[tree->free_list.size++] = idx;

    // Optional: scrub released node fields (helps catch use-after-free in debug)
    tree->nodes.data[idx].parent      = EM_NODE_NULL;
    tree->nodes.data[idx].prev        = EM_NODE_NULL;
    tree->nodes.data[idx].next        = EM_NODE_NULL;
    tree->nodes.data[idx].first_child = EM_NODE_NULL;
    tree->nodes.data[idx].last_child  = EM_NODE_NULL;

    return 0;
}

static void
em_link(em_tree* tree, em_idx node_idx, em_idx parent_idx, em_idx prev_idx, em_idx next_idx) {
    em_node* parent = &tree->nodes.data[parent_idx];
    em_node* node   = &tree->nodes.data[node_idx];

    node->parent = parent_idx;
    node->prev   = prev_idx;
    node->next   = next_idx;

    // Link prev -> node or parent.first_child
    if (prev_idx != EM_NODE_NULL) {
        tree->nodes.data[prev_idx].next = node_idx;
    } else {
        parent->first_child = node_idx;
    }

    // Link node -> next or parent.last_child
    if (next_idx != EM_NODE_NULL) {
        tree->nodes.data[next_idx].prev = node_idx;
    } else {
        parent->last_child = node_idx;
    }
}

static void em_unlink(em_tree* tree, em_idx node_idx) {
    em_node* node       = &tree->nodes.data[node_idx];
    em_idx   parent_idx = node->parent;
    if (parent_idx == EM_NODE_NULL) {
        node->prev = EM_NODE_NULL;
        node->next = EM_NODE_NULL;
        return;
    }

    em_node* parent   = &tree->nodes.data[parent_idx];
    em_idx   prev_idx = node->prev;
    em_idx   next_idx = node->next;

    // Fix prev neighbor or parent's first_child
    if (prev_idx != EM_NODE_NULL) {
        tree->nodes.data[prev_idx].next = next_idx;
    } else {
        parent->first_child = next_idx;
    }

    // Fix next neighbor or parent's last_child
    if (next_idx != EM_NODE_NULL) {
        tree->nodes.data[next_idx].prev = prev_idx;
    } else {
        parent->last_child = prev_idx;
    }

    node->prev = EM_NODE_NULL;
    node->next = EM_NODE_NULL;
}

static void
em_promote(em_tree* tree,
           em_idx   parent_idx,
           em_idx   prev_idx, // neighbor before removed node in parent's child list (or NULL)
           em_idx   next_idx, // neighbor after removed node in parent's child list (or NULL)
           em_idx   removed_first_child,
           em_idx   removed_last_child) {
    if (removed_first_child == EM_NODE_NULL)
        return;

    em_node* parent = &tree->nodes.data[parent_idx];
    em_node* first  = &tree->nodes.data[removed_first_child];
    em_node* last   = &tree->nodes.data[removed_last_child];

    // Connect prev neighbor to first child, or set parent.first_child
    if (prev_idx != EM_NODE_NULL) {
        tree->nodes.data[prev_idx].next = removed_first_child;
    } else {
        parent->first_child = removed_first_child;
    }
    first->prev = prev_idx;

    // Connect last child to next neighbor, or set parent.last_child
    if (next_idx != EM_NODE_NULL) {
        tree->nodes.data[next_idx].prev = removed_last_child;
    } else {
        parent->last_child = removed_last_child;
    }
    last->next = next_idx;

    // Update parent pointer for every promoted child in that sibling chain
    for (em_idx cur = removed_first_child; cur != EM_NODE_NULL; cur = tree->nodes.data[cur].next) {
        tree->nodes.data[cur].parent = parent_idx;
    }
}

/*
 * Public
 */

int em_tree_add(em_ctx* ctx, em_tree* tree, em_idx parent_idx) {
    if (!tree || !ctx)
        return -1;
    if (parent_idx == EM_NODE_NULL)
        return -1;
    if (parent_idx < 0 || parent_idx >= (em_idx)tree->nodes.size)
        return -1;

    em_idx new_idx = em_alloc_idx(ctx, tree);
    if (new_idx == EM_NODE_NULL)
        return -1;

    em_node* parent = &tree->nodes.data[parent_idx];
    em_node* node   = &tree->nodes.data[new_idx];

    node->parent      = parent_idx;
    node->prev        = EM_NODE_NULL;
    node->next        = EM_NODE_NULL;
    node->first_child = EM_NODE_NULL;
    node->last_child  = EM_NODE_NULL;

    // Append as last child
    if (parent->last_child == EM_NODE_NULL) {
        // Empty child list: first and last both become the new node
        parent->first_child = new_idx;
        parent->last_child  = new_idx;
    } else {
        em_idx last_idx                 = parent->last_child;
        node->prev                      = last_idx;
        tree->nodes.data[last_idx].next = new_idx;
        parent->last_child              = new_idx;
    }

    return 0;
}

// Detach from the sibling list but do NOT free/reuse the node idx.
int em_unlink_node(em_ctx* ctx, em_tree* tree, em_idx node_idx) {
    (void)ctx;
    if (!tree)
        return -1;
    if (node_idx < 0 || node_idx >= (em_idx)tree->nodes.size)
        return -1;

    em_unlink(tree, node_idx);
    return 0;
}

// Swap two nodes' positions in their sibling lists (can be different parents).
// This keeps each node's own subtree (children pointers) intact.
int em_tree_swap(em_tree* tree, em_idx a, em_idx b) {
    if (!tree)
        return -1;
    if (a == b)
        return 0;
    if (a < 0 || b < 0)
        return -1;
    if (a >= (em_idx)tree->nodes.size || b >= (em_idx)tree->nodes.size)
        return -1;

    em_node* a_node = &tree->nodes.data[a];
    em_node* b_node = &tree->nodes.data[b];

    // Snapshot original surrounding links
    em_idx a_parent = a_node->parent;
    em_idx a_prev   = a_node->prev;
    em_idx a_next   = a_node->next;

    em_idx b_parent = b_node->parent;
    em_idx b_prev   = b_node->prev;
    em_idx b_next   = b_node->next;

    // Unlink both nodes from their current sibling lists
    em_unlink(tree, a);
    em_unlink(tree, b);

    // When computing insertion neighbors, handle adjacency so we don't self-link.
    // If a's intended prev neighbor is actually a itself (because b_prev==a), replace with b.
    em_idx a_prev_ins = (b_prev == a) ? b : b_prev;
    em_idx a_next_ins = (b_next == a) ? b : b_next;

    em_idx b_prev_ins = (a_prev == b) ? a : a_prev;
    em_idx b_next_ins = (a_next == b) ? a : a_next;

    // Reinsert a where b was, and b where a was.
    if (b_parent != EM_NODE_NULL) {
        em_link(tree, a, b_parent, a_prev_ins, a_next_ins);
    } else {
        // If b had no parent, we can't insert into a list; leave a detached.
        a_node->parent = EM_NODE_NULL;
    }

    if (a_parent != EM_NODE_NULL) {
        em_link(tree, b, a_parent, b_prev_ins, b_next_ins);
    } else {
        b_node->parent = EM_NODE_NULL;
    }

    return 0;
}

// Remove a node and reuse its idx.
// Children are promoted into the removed node's position in the parent's child list.
int em_tree_remove(em_ctx* ctx, em_tree* tree, em_idx node_idx) {
    if (!tree || !ctx)
        return -1;
    if (node_idx < 0 || node_idx >= (em_idx)tree->nodes.size)
        return -1;

    em_node* node       = &tree->nodes.data[node_idx];
    em_idx   parent_idx = node->parent;

    // Capture neighbors in the sibling list BEFORE unlinking
    em_idx prev_idx = node->prev;
    em_idx next_idx = node->next;

    // Capture children range for promotion
    em_idx first_child = node->first_child;
    em_idx last_child  = node->last_child;

    // Detach node from its parent's sibling list
    em_unlink(tree, node_idx);

    // If the node had children, promote them into parent's child list where node was
    if (parent_idx != EM_NODE_NULL && first_child != EM_NODE_NULL) {
        em_promote(tree, parent_idx, prev_idx, next_idx, first_child, last_child);
    }

    // Now free node_idx for reuse
    return em_free_idx(ctx, tree, node_idx);
}

// These would be useful for widgets that are composed of multiple parts like a slider or text box.

// Adds the subtree to a specified node. Reordering of the nodes may be done to ensure it's within
// the appropiate constraints. The src_tree is destroyed afterwards
int em_add_subtree(em_ctx* ctx, em_idx dst_idx, em_tree* dst, em_tree* src) {
    // Reserve the space for the new tree to be merged.
    // Ensure that all the nodes are valid. If there are free indices, in theory it shouldn't matter
    // as traversal is done by the fields set in the node. To try and improve spatial locality this
    // will not use dst_tree's free_list to insert nodes. If there ever reaches a point where
    // there's way too many free nodes, its probably a sign to clean up the tree as a whole instead
    // of trying to insert them in the free spots.
    size_t required_size = src->nodes.size + dst->nodes.size;
    if (em_reserve(
            ctx, (void**)&dst->nodes.data, &dst->nodes.capacity, required_size, sizeof(em_node)) ==
        -1) {
        return -1;
    }

    return 0;
}

// Combines the contents of two trees by merging nodes with the same depth level within the trees.
int em_merge_subtree(em_ctx* ctx, em_idx dst_idx, em_tree* dst_tree, em_tree* src_tree) {
}

// Change this to allow allocating a specified number of nodes which allows creation of subtrees for
// template widgets.
int em_tree_init(em_ctx* ctx, struct em_tree* tree, size_t size) {
    tree->nodes.data     = (em_node*)ctx->allocator.alloc(sizeof(em_node) * DEFAULT_ARRAY_SIZE,
                                                          ctx->allocator.context);
    tree->nodes.size     = 0;
    tree->nodes.capacity = DEFAULT_ARRAY_SIZE;

    tree->free_list.data =
        (em_idx*)ctx->allocator.alloc(sizeof(em_idx) * DEFAULT_ARRAY_SIZE, ctx->allocator.context);
    tree->free_list.size     = 0;
    tree->free_list.capacity = DEFAULT_ARRAY_SIZE;
    return 0;
}

int em_tree_free(em_ctx* ctx, em_tree* tree) {
    ctx->allocator.dealloc((void*)tree->nodes.data, ctx->allocator.context);
    ctx->allocator.dealloc((void*)tree->free_list.data, ctx->allocator.context);
    return 0;
}
