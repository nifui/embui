#include <em/em_tree.h>
#include <em/em_type.h>

static inline int em_tree_valid(const em_tree* tree, em_idx idx) {
    return idx != EM_IDX_NULL && idx < tree->nodes.size;
}

em_result em_tree_init(em_tree* tree) {
    if (!tree) {
        return EM_ERR_INVALID_ARGUMENT;
    }
    tree->nodes.data     = NULL;
    tree->nodes.size     = 0;
    tree->nodes.capacity = 0;

    tree->free_list.data     = NULL;
    tree->free_list.size     = 0;
    tree->free_list.capacity = 0;

    return EM_OK;
}

void em_tree_destroy(em_ctx* ctx, em_tree* tree) {
#ifndef NO_ALLOCATOR
    if (!tree || !ctx)
        return;

    if (tree->nodes.data)
        ctx->allocator.free(tree->nodes.data, ctx->allocator.context);

    if (tree->free_list.data)
        ctx->allocator.free(tree->free_list.data, ctx->allocator.context);
#endif
}

// Method for a more efficient bullk add than multiple em_tree_add calls.

em_result
em_tree_bulk_add(em_ctx* ctx, em_tree* tree, em_idx parent, size_t amount, em_idx* handle_idxs) {
}

em_result em_tree_add(em_ctx* ctx, em_tree* tree, em_idx parent, em_idx* idx) {
    em_result res;
    if (!tree || !idx)
        return EM_ERR_INVALID_ARGUMENT;

    em_idx node_idx;

    if (tree->free_list.size) {
        node_idx = tree->free_list.data[--tree->free_list.size];
    } else {

        res = em_reserve(ctx,
                         (void**)&tree->nodes.data,
                         &tree->nodes.capacity,
                         tree->nodes.size + 1,
                         sizeof(em_node));
        EM_EXPECT(res);
        node_idx = tree->nodes.size++;
    }

    em_node node = {.parent      = parent,
                    .prev        = EM_IDX_NULL,
                    .next        = EM_IDX_NULL,
                    .first_child = EM_IDX_NULL,
                    .last_child  = EM_IDX_NULL,
                    .handle_idx  = EM_IDX_NULL};

    tree->nodes.data[node_idx] = node;

    if (parent != EM_IDX_NULL) {
        em_node* p = &tree->nodes.data[parent];

        if (p->last_child == EM_IDX_NULL) {
            p->first_child = node_idx;
            p->last_child  = node_idx;
        } else {
            em_node* last = &tree->nodes.data[p->last_child];

            last->next = node_idx;

            tree->nodes.data[node_idx].prev = p->last_child;

            p->last_child = node_idx;
        }
    }

    *idx = node_idx;

    return EM_OK;
}

static void em_tree_unlink(em_tree* tree, em_idx idx) {
    em_node* node = &tree->nodes.data[idx];

    if (node->parent != EM_IDX_NULL) {
        em_node* p = &tree->nodes.data[node->parent];

        if (p->first_child == idx)
            p->first_child = node->next;

        if (p->last_child == idx)
            p->last_child = node->prev;
    }

    if (node->prev != EM_IDX_NULL)
        tree->nodes.data[node->prev].next = node->next;

    if (node->next != EM_IDX_NULL)
        tree->nodes.data[node->next].prev = node->prev;

    node->parent = EM_IDX_NULL;
    node->prev   = EM_IDX_NULL;
    node->next   = EM_IDX_NULL;
}

em_result em_node_extract(em_ctx* ctx, em_tree* tree, em_idx target, em_node* out) {
    (void)ctx;

    if (!tree || !out)
        return EM_ERR_INVALID_ARGUMENT;

    if (!em_tree_valid(tree, target))
        return EM_ERR_INVALID_INDEX;

    *out = tree->nodes.data[target];

    em_tree_unlink(tree, target);

    return EM_OK;
}

em_result em_tree_remove(em_ctx* ctx, em_tree* tree, em_idx target) {
    (void)ctx;

    if (!tree)
        return EM_ERR_INVALID_ARGUMENT;

    if (!em_tree_valid(tree, target))
        return EM_ERR_INVALID_INDEX;

    em_tree_unlink(tree, target);

    em_node* n = &tree->nodes.data[target];

    em_idx child = n->first_child;

    while (child != EM_IDX_NULL) {
        tree->nodes.data[child].parent = EM_IDX_NULL;
        child                          = tree->nodes.data[child].next;
    }
    em_result res;
    res = em_reserve(ctx,
                     (void**)&*tree->free_list.data,
                     &tree->free_list.capacity,
                     tree->free_list.size + 1,
                     sizeof(em_idx));
    EM_EXPECT(res);
    EM_PUSH(tree->free_list, target);

    return EM_OK;
}

// Notes: This is recursive, rewrite as non recursive if possible.
em_result em_node_destroy(em_ctx* ctx, em_tree* tree, em_idx target) {
    if (!em_tree_valid(tree, target))
        return EM_ERR_INVALID_INDEX;

    em_idx child = tree->nodes.data[target].first_child;

    while (child != EM_IDX_NULL) {
        em_idx next = tree->nodes.data[child].next;

        EM_EXPECT(em_node_destroy(ctx, tree, child));

        child = next;
    }

    return em_tree_remove(ctx, tree, target);
}

em_result em_add_subtree(em_ctx* ctx, em_idx dst_idx, em_tree* dst_tree, em_tree* src_tree) {
    em_result res;
    // Since this will not use the free list to prevent fragmentation, the operation is a simple
    // offset increment of size for each field of each node.
    //  Then copy over the nodes and whatnot to the tree.
    // Finally link the subtree to dst_idx via a children or sibling.
    size_t required_size = src_tree->nodes.size + dst_tree->nodes.size;
    res                  = em_reserve(ctx,
                                      (void**)&dst_tree->nodes.data,
                                      &dst_tree->nodes.capacity,
                                      required_size,
                                      sizeof(em_node));
    EM_EXPECT(res);
    size_t offset = dst_tree->nodes.size;

    EM_MEMCPY(&dst_tree->nodes.data[offset],
              src_tree->nodes.data,
              src_tree->nodes.size * sizeof(em_node));

    // Fix up the copied nodes
    for (size_t i = 0; i < src_tree->nodes.size; i++) {
        em_node* node = &dst_tree->nodes.data[offset + i];

        if (node->parent != EM_IDX_NULL) {
            node->parent += offset;
        }
        if (node->next != EM_IDX_NULL) {
            node->next += offset;
        }
        if (node->prev != EM_IDX_NULL) {
            node->prev += offset;
        }
        if (node->first_child != EM_IDX_NULL) {
            node->first_child += offset;
        }
        if (node->last_child != EM_IDX_NULL) {
            node->last_child += offset;
        }
    }

    dst_tree->nodes.size += src_tree->nodes.size;
    return EM_OK;
}

em_result em_collect_parents(em_tree* tree,
                             em_ctx*  ctx,
                             em_idx   target,
                             em_node* dst_nodes,
                             size_t   dst_capacity,
                             size_t*  found_amount) {

    return EM_OK;
}

em_result em_node_swap(em_tree* tree, em_idx a, em_idx b) {
    if (!tree)
        return EM_ERR_INVALID_ARGUMENT;

    if (!em_tree_valid(tree, a) || !em_tree_valid(tree, b))
        return EM_ERR_INVALID_INDEX;

    em_node tmp         = tree->nodes.data[a];
    tree->nodes.data[a] = tree->nodes.data[b];
    tree->nodes.data[b] = tmp;

    return EM_OK;
}
