#include <em/em_event.h>

em_registry_value* em_lookup_registry(em_callback_registry* registry, em_idx handle_idx) {
    for (int i = 0; i < registry->values.size; i++) {
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

em_result
em_register_callback(em_ctx* ctx, em_callback_registry* registry, em_registry_value* value) {
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
    EM_PUSH(registry->values, *value);
    return EM_OK;
}
