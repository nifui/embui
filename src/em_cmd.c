#include <em/em_cmd.h>
#define EM_EMIT
#ifdef EM_EMIT
int em_emit_cmd(em_ctx* ctx, em_ui* ui) {
}
#elif defined(EM_FIXED)
int em_fixed_cmd() {
}
#elif defined(EM_CALL)
#include <em/em_draw.h>

int em_call_cmd(em_ctx* ctx, em_ui* ui, em_drawer* drawer) {
}
#endif
