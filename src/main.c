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
 * @idea Define a way to allow grouping of elements for quicker styling. Implement a macro that
 *       allows users to define a style and the associated group. Rough idea: Let users insert a
 *       handle into a group. This group can be targetted with specific styling. Users can create a
 *       file called style.c or whatnot that contains defines for specific groups. Instead ofa
 *       hashmap we could have the user define names attributed to a value. However this leaves a
 *       lot to the user getting right. Issue is with templates where the inside might be hidden
 * from the user. Maybe define a TAG macro?
 *
 *
 *
 * @optional Feature for allowing users to reserve types even with no references as a sort of
 * caching layer.
 *
 *
 *
 *
 * Notes if anything - Ignore
 *
 *
 * @why Handles - Since we want to avoid making repeated allocation calls due to the primary target
 * being embedded we should try to reuse already allocated memory as much as possible. Because of
 * how stability is needed for a majority of the resources, the simple approach is using a handle to
 * a resource reference. By hiding the internals of the resource_reference, the user cannot
 *      reference stale resources and is fully managed.
 *
 * @why Reference counting - Helps to track shared and unused resources. Without it fragmentation is
 * likely. Issues with ref counting is thread safety. Also resources that might need to be perserved
 * even after all items that reference it don't exist, require the user to explicitly indicate. If
 * the user forgets they reserved it, then the type can eat up memory. The user can easily miscall,
 * leading to a resource cleanup even when items still refer to it. The fix here might be creating
 * an explicit priority list for resources that should be kept, but leads to an extra layer of
 * indirection. The ideal fix is just have the user keep the important type.
 *
 * @why Free list - Required with reference counting.
 *
 * @why Transparent types - User allocation is a pain to figure out due to different types sizes and
 * overall API incompatility when adding new types. With transparent types, this is handled by the
 * compiler. Only issue is the user must not try to just modify the structs directly. Any other
 * workaround basically destroys the purpose of the opaque type so might as well make it
 * transparent.
 *
 *
 * */

/**
 * Define a way to implement animations, either allow specifying a set of keyframes via an array of
 * floats or implement custom curve fitting functions to properly interpolate between frames to give
 * a smooth look. Allow specifying properties to lerp between, etc.
 *
 * Handles never get reorderedd however the nodes within the tree can. By making handles act as the
 * stable reference we can reoder the tree with less information. Handles stay in order while the
 * tree may swap. Along with that we can add changes to the API without causing breaking changes as
 * em_handle is a single type.
 *
 *
 *  */

#include <em/em.h>

static em_node   nodes[64];
static em_handle handles[64];
static em_style  styles[16];

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
    em_ui_desc desc = {
        .nodes           = nodes,
        .node_capacity   = 64,
        .styles          = styles,
        .style_capacity  = 16,
        .handles         = handles,
        .handle_capacity = 64,
    };
    em_result res;
    em_ctx    ctx = {.allocator = allocator};
    em_ui     ui  = {0};
    em_handle root;
    res = em_init_ui(&ctx, &ui, &desc, &root);
    EM_EXPECT(res);

    res = em_add_prim(&ctx, &ui, EM_NODE_ROOT, DEFAULT_STYLE_IDX, RECT);
    EM_EXPECT(res);

    printf("%s", em_error_string(res));
    return 0;
}
