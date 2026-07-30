#include <em/em_cmd.h>

#ifdef EM_EMIT
int em_emit_cmd();

#elif defined(EM_FIXED)
int em_fixed_cmd();

#elif defined(EM_CALL)
// Requires drawer to have valid functions for the specified primitives.
#include <em/em_draw.h>

int em_call_cmd(em_ctx* ctx, em_ui* ui, em_drawer* drawer) {
}
#endif
