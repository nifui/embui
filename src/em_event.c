#include <em/em_event.h>
#include <em/em_tree.h>
#include <em/em_type.h>

em_registry_value* em_lookup_registry(em_callback_registry* registry, em_idx handle_idx) {
    for (size_t i = 0; i < registry->values.size; i++) {
        if (registry->values.data[i].handle_idx == handle_idx) {
            return &registry->values.data[i];
        }
    }
    return NULL;
}

em_result em_deregister_callback(em_callback_registry* registry, em_idx handle_idx) {
    for (size_t i = 0; i < registry->values.size; ++i) {
        if (registry->values.data[i].handle_idx == handle_idx) {
            registry->values.data[i] = registry->values.data[--registry->values.size];
            return EM_OK;
        }
    }
    return EM_ERR_INVALID_HANDLE;
}

em_result em_register_callback(em_ctx*               ctx,
                               em_callback_registry* registry,
                               em_registry_value*    value,
                               em_idx*               dst_idx) {
    if (registry->values.size >= registry->values.capacity) {
        size_t new_capacity = registry->values.capacity == 0 ? 2 : registry->values.capacity * 2;
        void*  res          = ctx->allocator.realloc(registry->values.data,
                                                     new_capacity * sizeof(em_registry_value),
                                                     ctx->allocator.context);
        if (!res) {
            return EM_ERR_OUT_OF_MEMORY;
        }
        registry->values.data     = (em_registry_value*)res;
        registry->values.capacity = new_capacity;
    }
    // !!! Might be dangerous as size_t converts to em_idx which can be any size specified by the
    //     user.
    *dst_idx = (em_idx)registry->values.size;
    EM_PUSH(registry->values, *value);
    return EM_OK;
}

em_result em_propogate_event(em_ctx*               ctx,
                             em_handle_pool*       handles,
                             em_callback_registry* registry,
                             em_event*             event,
                             em_idx                initial) {
    em_node   nodes[MAX_UI_DEPTH];
    size_t    found_amount;
    em_result res;
    res = em_collect_parents(ctx, initial, nodes, MAX_UI_DEPTH, &found_amount);
    if (res != EM_OK) {
        return res;
    }
    for (size_t i = 0; i < found_amount; i++) {
        em_handle handle = handles->data[nodes[i].handle_idx];
        if (handle.callback_idx != EM_IDX_NULL) {
            em_registry_value val = registry->values.data[handle.callback_idx];
            val.callback(val.state, *event);
            break;
        }
    }
    return EM_OK;
}
