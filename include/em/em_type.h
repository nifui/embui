/**
 * @file em_type.h
 * @brief Core types, allocator interface, vector helpers, and utility macros
 *        for the EM library.
 *
 * This header defines the fundamental types and utility macros used throughout
 * the library, including:
 *
 * - Index types and sentinel values
 * - Allocator abstraction
 * - Context object
 * - Simple vector helpers
 * - Error codes
 * - Dynamic memory reservation helper
 *   @invariant em_ctx can just be null and not contain anything. The fact that it's passed around
 * everywhere is just to allow allocation where needed.
 */

#pragma once
#ifndef EM_TYPE_H
#define EM_TYPE_H

#include <stdint.h>

#ifndef EM_MEMCPY
#include <string.h>

/**
 * @brief Memory copy implementation.
 *
 * May be overridden before including this header to provide a custom memcpy
 * implementation.
 */
#define EM_MEMCPY memcpy
#endif

#ifndef EM_IDX_TYPE

/**
 * @brief Integer type used for node and object indices.
 *
 * Override by defining `EM_IDX_TYPE` before including this header.
 */
#define EM_IDX_TYPE uint32_t
#endif

/**
 * @brief Index type used throughout the library.
 */
typedef EM_IDX_TYPE em_idx;

/**
 * @brief Sentinel representing an invalid node index.
 */
#define EM_NODE_NULL ((em_idx) - 1)

/**
 * @brief Sentinel representing an invalid generic index.
 */
#define EM_IDX_NULL ((em_idx) - 1)

/**
 * @brief Index of the root node.
 */
#define EM_NODE_ROOT 0

/**
 * @brief Index at which the default style for elements reside.
 */
#define DEFAULT_STYLE_IDX 0

/**
 * @brief Default stack size used by internal algorithms.
 */
#define STACK_SIZE 32

/**
 * @brief Default initial vector capacity.
 */
#define DEFAULT_ARRAY_SIZE 32

/**
 * @brief Max depth at which elements can be nested in the UI.
 *
 *
 *
 * */
#define MAX_UI_DEPTH 16u

/**
 * @brief User-provided memory allocator.
 *
 * All allocation performed by the library goes through this interface unless
 * `NO_ALLOCATOR` is defined.
 */
typedef struct em_allocator {

    /**
     * @brief Allocate a block of memory.
     *
     * @param size Number of bytes to allocate.
     * @param context User-defined allocator context.
     * @return Pointer to allocated memory, or NULL on failure.
     */
    void* (*alloc)(size_t size, void* context);

    /**
     * @brief Resize an existing allocation.
     *
     * May be NULL, in which case allocations are grown using
     * alloc/copy/free.
     *
     * @param ptr Existing allocation.
     * @param size Requested size in bytes.
     * @param context User-defined allocator context.
     * @return Reallocated memory or NULL on failure.
     */
    void* (*realloc)(void* ptr, size_t size, void* context);

    /**
     * @brief Free a previously allocated block.
     *
     * @param ptr Allocation to free.
     * @param context User-defined allocator context.
     */
    void (*free)(void* ptr, void* context);

    /**
     * @brief User-defined allocator state.
     */
    void* context;

} em_allocator;

/**
 * @brief Library context.
 *
 * Stores global state shared by library operations.
 */
typedef struct em_ctx {

    /**
     * @brief Memory allocator used by the library.
     */
    em_allocator allocator;

} em_ctx;

/**
 * @brief Checks if a value is an error value.
 *
 * @param res Variable at which the value is stored.
 */
#define EM_EXPECT(res)                                                                             \
    do {                                                                                           \
        if ((res) != EM_OK) {                                                                      \
            return (res);                                                                          \
        }                                                                                          \
    } while (0)

/**
 * @brief Macro to wave a function as uncomplete.
 *
 * @param msg Message to be printed
 */
#include <stdio.h>
#include <stdlib.h>

#define DO_PRAGMA(x) _Pragma(#x)
#define TODO(msg) DO_PRAGMA(message "TODO: " msg)
/**
 * @brief Declares a simple dynamically sized vector.
 *
 * Example:
 * @code
 * EM_VECTOR(int, values);
 * @endcode
 *
 * Expands to a structure containing:
 * - `data`
 * - `size`
 * - `capacity`
 *
 * @param type Element type.
 * @param name Variable name.
 */
#define EM_VECTOR(type, name)                                                                      \
    struct {                                                                                       \
        type*  data;                                                                               \
        size_t size;                                                                               \
        size_t capacity;                                                                           \
    } name

/**
 * @brief Append an element to a vector.
 *
 * The caller must ensure sufficient capacity exists.
 *
 * Typically:
 * @code
 * EM_VECTOR_RESERVE(...);
 * EM_PUSH(vec, value);
 * @endcode
 *
 * @param vec Vector variable.
 * @param item Element to append.
 */
#define EM_PUSH(vec, item)                                                                         \
    do {                                                                                           \
        (vec).data[(vec).size] = (item);                                                           \
        (vec).size++;                                                                              \
    } while (0)

/**
 * @brief Ensure a vector has at least the requested capacity.
 *
 * Expands to a call to ::em_reserve.
 *
 * @param ctx Library context.
 * @param vec Vector.
 * @param count Minimum required capacity.
 *
 * @return ::EM_OK on success or an error code.
 */

// clang-format off
#define EM_VECTOR_RESERVE(ctx, vec, count) \
    em_reserve(                            \
        ctx,                               \
        (void **)&(vec).data,              \
        &(vec).capacity,                   \
        count,                             \
        sizeof(*(vec).data),               \
    )
// clang-format on

/**
 * @brief Result codes returned by library functions.
 */
typedef enum {

    /** Operation completed successfully. */
    EM_OK = 0,

    /** Invalid function argument. */
    EM_ERR_INVALID_ARGUMENT,

    /** Invalid index*/
    EM_ERR_INVALID_INDEX,

    /** Memory allocation failed. */
    EM_ERR_OUT_OF_MEMORY,

    /** Resource pool has reached its maximum capacity. */
    EM_ERR_POOL_FULL,

    /** Required tree object is missing. */
    EM_ERR_MISSING_TREE,

    /** Invalid handle supplied. */
    EM_ERR_INVALID_HANDLE,

    /** Index or access outside valid bounds. */
    EM_ERR_OUT_OF_BOUNDS,

    /** The capacity of the provided storage was reached. */
    EM_ERR_CAPACITY,

} em_result;

/**
 * @brief Prints the error associated with the ::em_result variant.
 *
 *
 *
 * */
void em_print_result(em_result result) {
}

/**
 * @brief Ensure a dynamically allocated array has sufficient capacity.
 *
 * If the current capacity is smaller than @p min_capacity, the allocation is
 * grown using either the allocator's `realloc` function or an
 * alloc-copy-free sequence if `realloc` is unavailable.
 *
 * Capacity grows exponentially by doubling until the requested capacity is
 * reached.
 *
 * If `NO_ALLOCATOR` is defined, this function always returns
 * ::EM_ERR_OUT_OF_MEMORY when growth is required.
 *
 * @param ctx Library context containing the allocator.
 * @param data Pointer to the data pointer.
 * @param capacity Pointer to the current capacity.
 * @param min_capacity Minimum required number of elements.
 * @param elem_size Size of each element in bytes.
 *
 * @retval EM_OK Success.
 * @retval EM_ERR_OUT_OF_MEMORY Allocation failed.
 */
em_result
em_reserve(em_ctx* ctx, void** data, size_t* capacity, size_t min_capacity, size_t elem_size) {
    if (*capacity >= min_capacity) {
        return EM_OK;
    }

#ifdef NO_ALLOCATOR
    return EM_ERR_OUT_OF_MEMORY;
#else
    size_t new_capacity = *capacity ? *capacity : 1;

    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }

    void* res;

    if (!ctx->allocator.realloc) {

        res = ctx->allocator.alloc(new_capacity * elem_size, ctx->allocator.context);

        if (!res) {
            return EM_ERR_OUT_OF_MEMORY;
        }

        EM_MEMCPY(res, *data, (*capacity) * elem_size);

        ctx->allocator.free(*data, ctx->allocator.context);

    } else {

        res = ctx->allocator.realloc(*data, new_capacity * elem_size, ctx->allocator.context);

        if (!res) {
            return EM_ERR_OUT_OF_MEMORY;
        }
    }

    *data     = res;
    *capacity = new_capacity;

    return EM_OK;
#endif
}

#endif
