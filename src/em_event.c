#include <em/em_event.h>

em_result em_add_handler(em_ctx* ctx, em_handler handler, em_handlers* handlers, em_idx* dst_idx) {
    em_result res;
    res = em_reserve(ctx,
                     (void**)&handlers->inner.data,
                     &handlers->inner.capacity,
                     handlers->inner.size + 1,
                     sizeof(em_handler));
    EM_EXPECT(res);
    *dst_idx = handlers->inner.size;
    EM_PUSH(handlers->inner, handler);
    return EM_OK;
}

em_result em_remove_handler(em_ctx* ctx, em_handlers* handlers, em_idx handle_idx) {
    em_result res;
}
