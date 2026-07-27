/**
 * @file em_tree.h
 * @brief Tree data structure used by the UI hierarchy.
 *
 * The tree stores nodes in a contiguous pool indexed by em_idx.
 * Nodes are referenced by index rather than pointer to allow stable
 * storage during reallocation and efficient serialization.
 * @defgroup Tree Tree API
 *
 * ## Invariants
 *
 * - Every node has at most one parent.
 * - Siblings form a doubly-linked list.
 * - Children are stored in insertion order.
 * - Node indices remain stable until the node is removed.
 * - Removed indices may be reused through the free list.
 * - Handles are managed independently from tree structure.
 */
#pragma once
#ifndef EM_TREE_H
#define EM_TREE_H

#ifdef AUTOCOMPACT
#endif
#include "em_ui.h"
#include "em.h"

/**
 * @brief A node in an intrusive tree.
 *
 * Each node stores indices to its parent, siblings, and children.
 * Invalid references are represented by EM_INVALID_IDX.
 *
 * The node does not own the object referenced by @ref handle_idx.
 */
typedef struct em_node {
    em_idx parent;      ///< Parent node, or EM_INVALID_IDX if this is a root.
    em_idx prev;        ///< Previous sibling.
    em_idx next;        ///< Next sibling.
    em_idx first_child; ///< First child of this node.
    em_idx last_child;  ///< Last child of this node.
    em_idx handle_idx;  ///< Handle associated with this node.
} em_node;
typedef struct em_tree em_tree;
/**
 * @brief Creates a new node.
 *
 * Adds a node as the last child of the specified parent.
 *
 * @param ctx Memory allocation context.
 * @param tree Tree to modify.
 * @param parent Index of the parent node.
 * @param[out] idx Receives the index of the new node.
 *
 * @retval EM_OK Node was created successfully.
 * @retval EM_ERR_* Allocation or validation failed.
 */
em_result em_tree_add(em_ctx *ctx, em_tree *tree, em_idx parent, em_idx *idx);

/**
 * @brief Removes a node from the tree.
 *
 * The removed slot is placed on the tree's free list and may be reused
 * by subsequent insertions.
 *
 * Any children of the removed node are handled according to the tree's
 * removal policy.
 *
 * @param ctx Memory allocation context.
 * @param tree Tree to modify.
 * @param target Index of the node to remove.
 *
 * @return Operation status.
 */
em_result em_tree_remove(em_ctx *ctx, em_tree *tree, em_idx target);

/**
 * @brief Removes a node from the tree and returns its contents.
 *
 * The extracted node is detached from the tree without being destroyed.
 *
 * @param ctx Memory allocation context.
 * @param tree Tree to modify.
 * @param target Node to extract.
 * @param[out] out Receives the extracted node.
 *
 * @return Operation status.
 */
em_result em_node_extract(em_ctx *ctx, em_tree *tree, em_idx target, em_node *out);

/**
 * @brief Removes and destroys a node.
 *
 * Unlike em_node_extract(), the node is discarded and its storage becomes
 * available for reuse.
 *
 * @param ctx Memory allocation context.
 * @param tree Tree to modify.
 * @param target Node to destroy.
 *
 * @return Operation status.
 */
em_result em_node_destroy(em_ctx *ctx, em_tree *tree, em_idx target);

/**
 * @brief Swaps two nodes in the tree.
 *
 * All structural links are exchanged. The associated handles remain
 * attached to their original nodes.
 *
 * @param tree Tree containing the nodes.
 * @param a Index of the first node.
 * @param b Index of the second node.
 *
 * @return Operation status.
 */
em_result em_node_swap(em_tree *tree, em_idx a, em_idx b);

/**
 * @brief Appends an entire subtree.
 *
 * Copies or moves every node from the source tree into the destination
 * tree beneath the specified destination node.
 *
 * @param ctx Memory allocation context.
 * @param dst_idx Parent node in the destination tree.
 * @param dst_tree Destination tree.
 * @param src_tree Source tree.
 *
 * @return Operation status.
 */
em_result em_add_subtree(em_ctx *ctx, em_idx dst_idx, em_tree *dst_tree, em_tree *src_tree);

#endif
