/*
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

------------------------------------------------------------------------------
*/
#pragma once
#ifndef EM_H
#define EM_H

#include "em_math.h"
#include "em_real.h"
#include "em_draw.h"
#include "em_ui.h"
#include "em_pool.h"
#include "em_tree.h"
#include "em_event.h"
#endif
