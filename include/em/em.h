/*
 * @brief This only handles layout and depth, anything that's related to rendering is strictly
 * delegated to the user to implement. While a standard set of draw functions are provided a
 * framebuffer or something of the kind is needed.
 *
 *
@defgroup Configuration Define

These macros may be defined before including <em.h> to customize the library.
    @def EM_IDX_TYPE

    @brief Specifies the integer type used for resource indices.

    Default:
        uint32_t

    Valid values:
        uint8_t
        uint16_t
        uint32_t
        uint64_t

    Notes:
        - Smaller types reduce memory usage.
        - Limits the maximum number of addressable resources.
    Example:
        #define EM_IDX_TYPE uint16_t

    @def NO_ALLOCATOR

    @brief Specifies that an allocator is not supplied.

    Notes:
        - Mainly used in em_reserve.
        - If an attempt to use an allocator happens an error gets returned.

    @def MAX_UI_DEPTH

    @brief Controls the maximum depth of the tree. Can be greater than the actual depth.


    @invariant Not strictly enforced.
    @invariant If the actual depth is greater than the one specified here, it'll miss nodes and not
        output commands for them properly.

    Default: 16u

    Valid values:
        Dependent on target, utilizes the unisgned datatype.

    Notes:
        If you need anything above 100, consdier rewriting your UI.

    Example:
        #define MAX_UI_DEPTH 10u

    @def USE_INT16

    @brief Controls the size of ints. If defined will use the int16_t type in places where ints are
           used.

    @invariant If being used must not exceed the int16 limit.



    @defgroup To Do

    @invariant - Replace @returns with @retval as majority return em_result with different error
                 types.
*/
#pragma once
#ifndef EM_H
#define EM_H

#include "em_math.h"
#include "em_ui.h"
#include "em_tree.h"
#include "em_event.h"
#endif
