#include <em/em_elements.h>

em_result
em_add_primitive(em_ctx* ctx, em_primitives* primitives, em_primitive primitive, em_idx* prim_idx) {
    em_result res;
    res = em_reserve(ctx,
                     (void**)&primitives->data,
                     &primitives->capacity,
                     primitives->size + 1,
                     sizeof(em_primitive));
    EM_EXPECT(res);
    *prim_idx = primitives->size;
    EM_PUSH(*primitives, primitive);
    return EM_OK;
}
