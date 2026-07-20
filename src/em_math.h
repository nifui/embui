/*
    em_math.h - single-header 2D math for embedded targets

    Configuration
    -------------

    Before including this header, define the types you want generated:

        #define EM_ENABLE_INT
        #define EM_ENABLE_FLOAT
        #define EM_ENABLE_UNSIGNED

        #include "em_math.h"

    Optional Bezier support (each requires its base type above):

        EM_ENABLE_BEZIER_INT
        EM_ENABLE_BEZIER_FLOAT
        EM_ENABLE_BEZIER_UNSIGNED

    Linkage / implementation
    ------------------------

    By default this header is fully self-contained: every function is
    `static inline` and defined right where it's declared. This is the default
    because it's the simplest thing to get right on an embedded toolchain.

    If you'd rather compile the math functions once and link them (e.g.
    to avoid duplicated code across many translation units on a
    flash-constrained part), override the linkage yourself:

        #define EM_API extern
        #include "em_math.h"   // in every file that uses the API

    and in exactly one source file:

        #define EM_API extern
        #define EM_MATH_IMPLEMENTATION
        #include "em_math.h"

    Custom float type
    -----------------

    EM_FLOAT_TYPE controls the type used for scalar/interpolation
    parameters (t, s) and for the "f"-suffixed vec2/rect/bezier variants.
    This is the main hook for platforms without hardware float support.

    Swap in double:

        #define EM_FLOAT_TYPE double
        #define EM_SQRT       sqrt

    Or swap in a fixed-point / soft-float type entirely, as long as it
    also defines matching arithmetic:

        #define EM_FLOAT_TYPE      q16_16
        #define EM_SQRT            q16_16_sqrt
        #define EM_FLOAT_MUL(a, b) q16_16_mul(a, b)
        #define EM_FLOAT_ADD(a, b) q16_16_add(a, b)
        #define EM_FLOAT_SUB(a, b) q16_16_sub(a, b)

    EM_FLOAT_MUL/ADD/SUB default to plain C operators, which is correct
    for float, double, or any other type where + - * already does the
    right thing (this includes most fixed-point typedefs backed by a
    wider intermediate, but NOT types needing a post-multiply shift --
    for those you must override EM_FLOAT_MUL).
*/
#pragma once
#ifndef EM_MATH_H
#define EM_MATH_H

#include <math.h>
#include <stdint.h>

/*
   Linkage
*/
#ifndef EM_API
#define EM_API static inline
#define EM_MATH_HEADER_ONLY 1
#endif
#ifndef EM_MATH_HEADER_ONLY
#define EM_MATH_HEADER_ONLY 0
#endif

#if EM_MATH_HEADER_ONLY
#define EM_MATH_EMIT_IMPL 1
#elif defined(EM_MATH_IMPLEMENTATION)
#define EM_MATH_EMIT_IMPL 1
#else
#define EM_MATH_EMIT_IMPL 0
#endif

/*
   Float type + arithmetic hooks
*/
#ifndef EM_FLOAT_TYPE
#define EM_FLOAT_TYPE float
#endif
typedef EM_FLOAT_TYPE em_float;

#ifndef EM_SQRT
#define EM_SQRT sqrtf
#endif

#ifndef EM_FLOAT_MUL
#define EM_FLOAT_MUL(a, b) ((a) * (b))
#endif
#ifndef EM_FLOAT_ADD
#define EM_FLOAT_ADD(a, b) ((a) + (b))
#endif
#ifndef EM_FLOAT_SUB
#define EM_FLOAT_SUB(a, b) ((a) - (b))
#endif

/*
   vec2
*/
#define EM_DECLARE_VEC2(type, suffix)                                                              \
    typedef struct {                                                                               \
        type x;                                                                                    \
        type y;                                                                                    \
    } em_vec2##suffix;                                                                             \
                                                                                                   \
    EM_API em_vec2##suffix em_vec2##suffix##_add(em_vec2##suffix a, em_vec2##suffix b);            \
    EM_API em_vec2##suffix em_vec2##suffix##_sub(em_vec2##suffix a, em_vec2##suffix b);            \
    EM_API em_vec2##suffix em_vec2##suffix##_scale(em_vec2##suffix v, em_float s);                 \
    EM_API em_vec2##suffix em_vec2##suffix##_lerp(                                                 \
        em_vec2##suffix a, em_vec2##suffix b, em_float t);                                         \
    EM_API type     em_vec2##suffix##_dot(em_vec2##suffix a, em_vec2##suffix b);                   \
    EM_API em_float em_vec2##suffix##_length(em_vec2##suffix v);

#define EM_IMPLEMENT_VEC2(type, suffix)                                                            \
    EM_API em_vec2##suffix em_vec2##suffix##_add(em_vec2##suffix a, em_vec2##suffix b) {           \
        return (em_vec2##suffix){(type)(a.x + b.x), (type)(a.y + b.y)};                            \
    }                                                                                              \
                                                                                                   \
    EM_API em_vec2##suffix em_vec2##suffix##_sub(em_vec2##suffix a, em_vec2##suffix b) {           \
        return (em_vec2##suffix){(type)(a.x - b.x), (type)(a.y - b.y)};                            \
    }                                                                                              \
                                                                                                   \
    EM_API em_vec2##suffix em_vec2##suffix##_scale(em_vec2##suffix v, em_float s) {                \
        return (em_vec2##suffix){(type)EM_FLOAT_MUL(v.x, s), (type)EM_FLOAT_MUL(v.y, s)};          \
    }                                                                                              \
                                                                                                   \
    EM_API em_vec2##suffix em_vec2##suffix##_lerp(                                                 \
        em_vec2##suffix a, em_vec2##suffix b, em_float t) {                                        \
        return (em_vec2##suffix){                                                                  \
            (type)EM_FLOAT_ADD(a.x, EM_FLOAT_MUL(EM_FLOAT_SUB(b.x, a.x), t)),                      \
            (type)EM_FLOAT_ADD(a.y, EM_FLOAT_MUL(EM_FLOAT_SUB(b.y, a.y), t))};                     \
    }                                                                                              \
                                                                                                   \
    EM_API type em_vec2##suffix##_dot(em_vec2##suffix a, em_vec2##suffix b) {                      \
        return (type)(a.x * b.x + a.y * b.y);                                                      \
    }                                                                                              \
                                                                                                   \
    EM_API em_float em_vec2##suffix##_length(em_vec2##suffix v) {                                  \
        return EM_SQRT((em_float)EM_FLOAT_ADD(EM_FLOAT_MUL(v.x, v.x), EM_FLOAT_MUL(v.y, v.y)));    \
    }

/*
   rect
*/
#define EM_DECLARE_RECT(type, suffix)                                                              \
    typedef struct {                                                                               \
        type x;                                                                                    \
        type y;                                                                                    \
        type width;                                                                                \
        type height;                                                                               \
    } em_rect##suffix;                                                                             \
                                                                                                   \
    EM_API int em_rect##suffix##_contains(em_rect##suffix r, em_vec2##suffix p);                   \
    EM_API int em_rect##suffix##_intersects(em_rect##suffix a, em_rect##suffix b);

#define EM_IMPLEMENT_RECT(type, suffix)                                                            \
    EM_API int em_rect##suffix##_contains(em_rect##suffix r, em_vec2##suffix p) {                  \
        return p.x >= r.x && p.x <= (type)(r.x + r.width) && p.y >= r.y &&                         \
               p.y <= (type)(r.y + r.height);                                                      \
    }                                                                                              \
                                                                                                   \
    EM_API int em_rect##suffix##_intersects(em_rect##suffix a, em_rect##suffix b) {                \
        return a.x <= (type)(b.x + b.width) && (type)(a.x + a.width) >= b.x &&                     \
               a.y <= (type)(b.y + b.height) && (type)(a.y + a.height) >= b.y;                     \
    }

/*
   bezier (linear / quadratic / cubic), built from vec2 primitives
*/
#define EM_DECLARE_BEZIER(type, suffix)                                                            \
    typedef struct {                                                                               \
        em_vec2##suffix p0;                                                                        \
        em_vec2##suffix p1;                                                                        \
    } em_bezier_linear##suffix;                                                                    \
    typedef em_bezier_linear##suffix em_line##suffix;                                              \
                                                                                                   \
    typedef struct {                                                                               \
        em_vec2##suffix p0;                                                                        \
        em_vec2##suffix p1;                                                                        \
        em_vec2##suffix p2;                                                                        \
    } em_bezier_quad##suffix;                                                                      \
                                                                                                   \
    typedef struct {                                                                               \
        em_vec2##suffix p0;                                                                        \
        em_vec2##suffix p1;                                                                        \
        em_vec2##suffix p2;                                                                        \
        em_vec2##suffix p3;                                                                        \
    } em_bezier_cubic##suffix;                                                                     \
                                                                                                   \
    EM_API em_vec2##suffix em_bezier_linear##suffix##_eval(em_bezier_linear##suffix curve,         \
                                                           em_float                 t);            \
    EM_API em_vec2##suffix em_bezier_quad##suffix##_eval(em_bezier_quad##suffix curve,             \
                                                         em_float               t);                \
    EM_API em_vec2##suffix em_bezier_cubic##suffix##_eval(em_bezier_cubic##suffix curve,           \
                                                          em_float                t);              \
    EM_API em_vec2##suffix em_bezier_linear##suffix##_derivative(em_bezier_linear##suffix curve,   \
                                                                 em_float                 t);      \
    EM_API em_vec2##suffix em_bezier_quad##suffix##_derivative(em_bezier_quad##suffix curve,       \
                                                               em_float               t);          \
    EM_API em_vec2##suffix em_bezier_cubic##suffix##_derivative(em_bezier_cubic##suffix curve,     \
                                                                em_float                t);

#define EM_IMPLEMENT_BEZIER(type, suffix)                                                          \
    EM_API em_vec2##suffix em_bezier_linear##suffix##_eval(em_bezier_linear##suffix curve,         \
                                                           em_float                 t) {           \
        return em_vec2##suffix##_lerp(curve.p0, curve.p1, t);                                      \
    }                                                                                              \
                                                                                                   \
    EM_API em_vec2##suffix em_bezier_linear##suffix##_derivative(em_bezier_linear##suffix curve,   \
                                                                 em_float                 t) {     \
        (void)t;                                                                                   \
        return em_vec2##suffix##_sub(curve.p1, curve.p0);                                          \
    }                                                                                              \
                                                                                                   \
    EM_API em_vec2##suffix em_bezier_quad##suffix##_eval(em_bezier_quad##suffix curve,             \
                                                         em_float               t) {               \
        em_vec2##suffix a = em_vec2##suffix##_lerp(curve.p0, curve.p1, t);                         \
        em_vec2##suffix b = em_vec2##suffix##_lerp(curve.p1, curve.p2, t);                         \
        return em_vec2##suffix##_lerp(a, b, t);                                                    \
    }                                                                                              \
                                                                                                   \
    EM_API em_vec2##suffix em_bezier_quad##suffix##_derivative(em_bezier_quad##suffix curve,       \
                                                               em_float               t) {         \
        em_vec2##suffix d0  = em_vec2##suffix##_sub(curve.p1, curve.p0);                           \
        em_vec2##suffix d1  = em_vec2##suffix##_sub(curve.p2, curve.p1);                           \
        em_vec2##suffix mid = em_vec2##suffix##_lerp(d0, d1, t);                                   \
        return em_vec2##suffix##_scale(mid, (em_float)2);                                          \
    }                                                                                              \
                                                                                                   \
    EM_API em_vec2##suffix em_bezier_cubic##suffix##_eval(em_bezier_cubic##suffix curve,           \
                                                          em_float                t) {             \
        em_vec2##suffix a = em_vec2##suffix##_lerp(curve.p0, curve.p1, t);                         \
        em_vec2##suffix b = em_vec2##suffix##_lerp(curve.p1, curve.p2, t);                         \
        em_vec2##suffix c = em_vec2##suffix##_lerp(curve.p2, curve.p3, t);                         \
        em_vec2##suffix d = em_vec2##suffix##_lerp(a, b, t);                                       \
        em_vec2##suffix e = em_vec2##suffix##_lerp(b, c, t);                                       \
        return em_vec2##suffix##_lerp(d, e, t);                                                    \
    }                                                                                              \
                                                                                                   \
    EM_API em_vec2##suffix em_bezier_cubic##suffix##_derivative(em_bezier_cubic##suffix curve,     \
                                                                em_float                t) {       \
        em_vec2##suffix d0  = em_vec2##suffix##_sub(curve.p1, curve.p0);                           \
        em_vec2##suffix d1  = em_vec2##suffix##_sub(curve.p2, curve.p1);                           \
        em_vec2##suffix d2  = em_vec2##suffix##_sub(curve.p3, curve.p2);                           \
        em_vec2##suffix a   = em_vec2##suffix##_lerp(d0, d1, t);                                   \
        em_vec2##suffix b   = em_vec2##suffix##_lerp(d1, d2, t);                                   \
        em_vec2##suffix mid = em_vec2##suffix##_lerp(a, b, t);                                     \
        return em_vec2##suffix##_scale(mid, (em_float)3);                                          \
    }

/*
   Instantiation. type/suffix pairs, gated by the EM_ENABLE_* switches.
*/
EM_DECLARE_VEC2(int, i)
EM_DECLARE_RECT(int, i)
#if EM_MATH_EMIT_IMPL
EM_IMPLEMENT_VEC2(int, i)
EM_IMPLEMENT_RECT(int, i)
#endif
#define EM_VEC2_i_EXISTS

#ifdef EM_ENABLE_FLOAT
EM_DECLARE_VEC2(em_float, f)
EM_DECLARE_RECT(em_float, f)
#if EM_MATH_EMIT_IMPL
EM_IMPLEMENT_VEC2(em_float, f)
EM_IMPLEMENT_RECT(em_float, f)
#endif
#define EM_VEC2_f_EXISTS
#endif

#ifdef EM_ENABLE_UNSIGNED
EM_DECLARE_VEC2(unsigned, u)
EM_DECLARE_RECT(unsigned, u)
#if EM_MATH_EMIT_IMPL
EM_IMPLEMENT_VEC2(unsigned, u)
EM_IMPLEMENT_RECT(unsigned, u)
#endif
#define EM_VEC2_u_EXISTS
#endif
#define EM_ENABLE_BEZIER_INT
#ifdef EM_ENABLE_BEZIER_INT
#ifndef EM_VEC2_i_EXISTS
#error "EM_ENABLE_BEZIER_INT requires EM_ENABLE_INT."
#endif
EM_DECLARE_BEZIER(int, i)
#if EM_MATH_EMIT_IMPL
EM_IMPLEMENT_BEZIER(int, i)
#endif
#endif

#ifdef EM_ENABLE_BEZIER_FLOAT
#ifndef EM_VEC2_f_EXISTS
#error "EM_ENABLE_BEZIER_FLOAT requires EM_ENABLE_FLOAT."
#endif
EM_DECLARE_BEZIER(em_float, f)
#if EM_MATH_EMIT_IMPL
EM_IMPLEMENT_BEZIER(em_float, f)
#endif
#endif

#ifdef EM_ENABLE_BEZIER_UNSIGNED
#ifndef EM_VEC2_u_EXISTS
#error "EM_ENABLE_BEZIER_UNSIGNED requires EM_ENABLE_UNSIGNED."
#endif
EM_DECLARE_BEZIER(unsigned, u)
#if EM_MATH_EMIT_IMPL
EM_IMPLEMENT_BEZIER(unsigned, u)
#endif
#endif

#endif /* EM_MATH_H */
