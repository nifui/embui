/**
 *
 * @todo Identify the entry point and do null checks there.
 *
 * @todo Remove the random null checks that are littered around in random functions.
 *
 * @todo Rewrite several header files to hide important types that should not be directly modified.
 *
 * @todo Add validation methods to properly construct the types.
 *
 * @todo Maybe switch to errno style of handling, but the current style might be good for
 *       multithreading scenarios where each thread can fail differently.
 *
 * @todo Fix the randomly littered invariant tags that aren't actually invariants.
 *
 * @todo There are probably cases where the operations are done in the wrong order which could lead
 *       to resource leaking or malformed memory.
 *
 * @todo There is dumb typing that I threw around like size_t, uint32_t that might not cast
 *       properly. Also wastes memory that isn't needed.
 *
 * @todo  Possibility that some of the operations could be implemented more efficienctly in terms of
 *        the algorithm used.
 *
 * */

/**
 * Define a way to implement animations, either allow specifying a set of keyframes via an array of
 * floats or implement custom curve fitting functions to properly interpolate between frames to give
 * a smooth look. Allow specifying properties to lerp between, etc.
 *
 *
 *
 *
 *  */

#include <em/em.h>

void myfree(void* ptr, void* context) {
    free(ptr);
}

void* myalloc(size_t size, void* context) {
    return malloc(size);
}

void* myrealloc(void* ptr, size_t new, void* context) {
    return realloc(ptr, new);
}

int main() {
    em_allocator allocator = {
        .free    = myfree,
        .alloc   = myalloc,
        .realloc = myrealloc,
        .context = NULL,
    };

    em_ctx ctx = {.allocator = allocator};
    em_ui  ui  = {0};
    em_add_prim(&ctx, &ui, EM_NODE_ROOT, DEFAULT_STYLE_IDX, RECT);

    return 0;
}
