/**
 * @file em_real.h
 * @brief Compile-time configuration of the library's real number type.
 *
 * By default, the library uses `float` as its real number type.
 *
 * To use a custom type (for example, a fixed-point or arbitrary-precision
 * type), define @ref EM_REAL before including this header. When overriding
 * @ref EM_REAL, you must also provide implementations of the arithmetic
 * macros documented below.
 *
 * Example:
 * @code
 * #define EM_REAL my_real_t
 * #define EM_REAL_ADD(a, b) my_add(a, b)
 * #define EM_REAL_SUB(a, b) my_sub(a, b)
 * #define EM_REAL_MUL(a, b) my_mul(a, b)
 * #define EM_REAL_DIV(a, b) my_div(a, b)
 *
 * #include "em_real.h"
 * @endcode
 */

/**
 * @defgroup config Configuration
 * @brief Compile-time configuration options.
 * @{
 */

/**
 * @def EM_REAL
 * @brief The scalar type used throughout the library.
 *
 * Defaults to `float`.
 *
 * If this macro is defined before including `em_real.h`, the library will use
 * the specified type instead.
 *
 * When overriding this macro, you must also define:
 * - ::EM_REAL_ADD
 * - ::EM_REAL_SUB
 * - ::EM_REAL_MUL
 * - ::EM_REAL_DIV
 */

#ifndef EM_REAL
#define EM_REAL float
#endif

/**                                                                                         \
 * @def EM_REAL_ADD(x, y)                                                                   \
 * @brief Adds two values of type ::EM_REAL.                                                \
 */

#ifndef EM_REAL_ADD
#define EM_REAL_ADD(x, y) ((x) + (y))
#endif

/**
 * @def EM_REAL_SUB(x, y)
 * @brief Subtracts two values of type ::EM_REAL.
 */

#ifndef EM_REAL_SUB
#define EM_REAL_SUB(x, y) ((x) - (y))
#endif

/**
 * @def EM_REAL_MUL(x, y)
 * @brief Multiplies two values of type ::EM_REAL.
 */

#ifndef EM_REAL_MUL
#define EM_REAL_MUL(x, y) ((x) * (y))
#endif

/**
 * @def EM_REAL_DIV(x, y)
 * @brief Divides two values of type ::EM_REAL.
 */

#ifndef EM_REAL_DIV
#define EM_REAL_DIV(x, y) ((x) / (y))
#endif

/** @} */
