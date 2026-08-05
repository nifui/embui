#pragma once
#ifndef EM_ELEMENTS_H
#define EM_ELEMENTS_H
#include "em_math.h"
#include "em_type.h"
// The main macro could perform substitution of a macro call with a single parameter and add another
// or something.
//
// List ot elements.
//
#define LIST(x)

/**
 * @brief Enum tag for em_prim
 */
typedef enum em_primitive_type {
    RECT,
    TEXT,
    LINE,
    CIRCLE,
} em_primitive_type;

#ifdef USE_INT16
// 16 bytes
typedef em_recti16 em_rect;
// 16 byte
typedef em_linei16 em_line;
// 8 bytes
typedef em_vec2i16 em_vec2;

#else
// 8 bytes
typedef em_recti em_rect;
// 8 bytes
typedef em_linei em_line;
// 4 bytes
typedef em_vec2i em_vec2;

#endif
// 4 butes
typedef em_vec2 em_point;

/**
 * @brief Representation of a circle.
 */
typedef struct em_circle {
    em_vec2 center;
    int     radius;
} em_circle;

/**
 * @brief Pointer to a string.
 */
typedef struct em_text {
    const char** text;
} em_text;

/**
 * @brief Primitive for UI drawing
 */
typedef struct em_primitive {
    em_primitive_type type;

    union {
        em_rect   r;
        em_line   l;
        em_circle c;
        em_point  p;
        em_text   t;
    };
} em_primitive;

// Calculates the axis alligned bounding box of a primitive based off its type.
// The bounding-box will be calculated to the smallest possible.
// If the bbox are used to determine collision certain cases like a line or point should use
// specially made function that are much more efficient than a bbox calculation.
// This assumes that the primitive was transformed prior and will not perform any transforms like
// rotations.
em_rect em_get_aabb(em_primitive* primitive);

typedef EM_VECTOR(em_primitive, em_primitives);

#endif
