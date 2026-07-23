#include "em_ui.h"
#include "em_tree.h"
#include "em_pool.h"

// The em_ctx is seperated from the UI, to allow it to be reused across multiple UIs.
typedef struct em_ui {
    em_tree*          tree;
    em_resource_pool* pool;
} em_ui;

int em_init_ui(struct em_ui* ui) {
}

// For each element/widget types a function is defined that adds into the em_resource_pool and
// registers a handle it gets back from the tree after node creation. The handle is then returned
// for the user. When the user wants to add state they can register a void *state  with some sort of
// handle registry. If the specified node's handle detexts an event acting on it, it'll call a
// function that gets the void * state and then pass it into another function for the user to handle
// the event.
// That way the user never has to explictly perform a search up of the assocaited widget state.
// Since handle indices are stable most of the time we can use it as a sort of key into the
// associated states and callbacks.

int em_add_circle() {
}

int em_add_rect() {
}

int em_add_line() {
}

int em_add_text() {
}

typedef enum em_event_type { TEMP } em_event_type;

typedef struct em_event {
    em_event_type type;

    // Could use a bit flag for buttons.
    union {
        int mouse_flags;
    };
} em_event;

typedef void (*callback)(void* state, em_event event);

// Gets the state that was linked to a specific handle.
int em_registry_state() {
}

// Gets the callback that was linked to a specific handle.
int em_registry_callback() {
}

// Multiple callbacks could refer to the same state if needed. TO account for this maybe use an
// index for each state and indices could be reused to index into the state array. Callbacks are
// more or less unique though.
typedef struct {
    em_idx state_idx;
    void (*callback)(void* state, em_event event);
} em_registry_value;

// Might aswell just use linear search, hashmaps are pretty expensive for a small amount of items.
typedef struct {
    // Weakly linked due to sharing of references.
    EM_VECTOR(void*, states);
    // Both of these two fields should always have the same size and capacity.
    EM_VECTOR(em_registry_value, registry_values);
    EM_VECTOR(em_idx, value_idx);
} em_callback_registry;

// Since the fields inside a em_handle cannot each be guranteed to be unique unless we do some
// hashing we might aswell use the handle_index which is guaranteed to be unique for each node.
// A BST could be used as the registry grows and searches become slower.
em_registry_value* em_lookup_registry(em_callback_registry* registry, em_idx handle_idx) {
    for (int i = 0; i < registry->value_idx.size; i++) {
        if (registry->value_idx.data[i] == handle_idx) {
            return &registry->registry_values.data[i];
        }
    }
    return NULL;
}

// Since cache locality is not a worry as we're jumping with fptrs and there exists a stable index
// to index into the array reordering is fine and we can shift the element at the end of the list
// into the removed spot.
int em_deregister_value(em_callback_registry* registry, em_idx value_idx) {
    if (value_idx >= registry->value_idx.size) {
        return -1;
    }
    registry->value_idx.size--;
    registry->registry_values.size--;
    registry->value_idx.data[value_idx] = registry->value_idx.data[registry->value_idx.size];
    registry->registry_values.data[value_idx] =
        registry->registry_values.data[registry->registry_values.size];
}

// Simple push operation as the vec is always guaranteed to be contiguous due to the removal
// strategy
int em_register_value(em_ctx* ctx, em_callback_registry* registry, em_registry_value* value) {
    if (registry->value_idx.size >= registry->value_idx.capacity) {
        size_t new_capacity = registry->value_idx.capacity * 2;
        void*  res =
            ctx->allocator.realloc(registry->value_idx.data, new_capacity, ctx->allocator.context);
        if (!res) {
            return -1;
        }
        registry->value_idx.capacity = new_capacity;
    }
}

// Registers a stateful widget
// The widget can be represented as a subtree.
int em_register_stateful(em_ui*, void* state, callback handler) {
}

// Just take a mouse event and do a binary search basically. However this assumes that elements
// aren't just going to overlap with multiple elements. Along with that visibility of the element
// has to be tested as well.
int em_add_button() {
}

// Whe emitting commands, it figures out how to layout the elements based on it's siblings and
// its parents. Children's tend not to inherent parents unless explicitly specified.
