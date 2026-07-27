#pragma once
#ifndef EM_TYPE_H
#define EM_TYPE_H

#include <stdint.h>

#ifndef EM_MEMCPY
#include <string.h>
#define EM_MEMCPY memcpy
#endif

#ifndef EM_IDX_TYPE
#define EM_IDX_TYPE uint32_t
#endif

typedef EM_IDX_TYPE em_idx;

#define EM_NODE_NULL ((em_idx) - 1)
#define EM_IDX_NULL ((em_idx) - 1)
#define EM_NODE_ROOT 0
#define STACK_SIZE 32
#define DEFAULT_ARRAY_SIZE 32

typedef struct em_allocator {
    void *(*alloc)(size_t size, void *context);
    void *(*realloc)(void *ptr, size_t size, void *context);
    void (*free)(void *ptr, void *context);
    void *context;
} em_allocator;

typedef struct em_ctx {
    em_allocator allocator;
} em_ctx;

#define EM_VECTOR(type, name) \
    struct {                  \
        type  *data;          \
        size_t size;          \
        size_t capacity;      \
    } name

#define EM_PUSH(vec, item)               \
    do {                                 \
        (vec).data[(vec).size] = (item); \
        (vec).size++;                    \
    } while (0)

// clang-format off
#define EM_VECTOR_RESERVE(ctx, vec, count) \
    em_reserve(                             \
        ctx,                                \
        (void **)&(vec).data,               \
        &(vec).capacity,                    \
        count,                              \
        sizeof(*(vec).data),                \
        (vec).size                          \
    )
// clang-format on

typedef enum {
    EM_OK = 0,
    EM_ERR_INVALID_ARGUMENT,
    EM_ERR_OUT_OF_MEMORY,
    EM_ERR_POOL_FULL,
    EM_ERR_MISSING_TREE,
    EM_ERR_INVALID_HANDLE,
    EM_ERR_OUT_OF_BOUNDS,
} em_result;

static em_result
em_reserve(em_ctx *ctx, void **data, size_t *capacity, size_t min_capacity, size_t elem_size) {
    if (*capacity >= min_capacity) {
        return EM_OK;
    }
    size_t new_capacity = *capacity ? *capacity : 1;
    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }
    void *res;
    if (!ctx->allocator.realloc) {
        res = ctx->allocator.alloc(new_capacity * elem_size, ctx->allocator.context);
        if (!res) {
            return EM_ERR_OUT_OF_MEMORY;
        }
        EM_MEMCPY(res, data, *capacity);
        ctx->allocator.free(data, ctx->allocator.context);
    } else {
        res = ctx->allocator.realloc(*data, new_capacity * elem_size, ctx->allocator.context);

        if (!res) {
            return EM_ERR_OUT_OF_MEMORY;
        }
    }
    *data     = res;
    *capacity = new_capacity;
    return EM_OK;
}

#endif
