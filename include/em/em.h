/*
------------------------------------------------------------------------------
Usage


Configuration

These macros may be defined before including <em.h> to customize the library.
    EM_IDX_TYPE

    Specifies the integer type used for resource indices.

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
