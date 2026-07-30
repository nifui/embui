/**
 * @file em_ui.h
 * @brief Types that are in a pool managed by a central resource pool.
 *
 * Contains pools that are indexed into by the handles. Operations here rely on handles given to the
 * user to be supplied to modify a target element's properties (primitive, styling, callbacks)
 * @defgroup Pool Pool API
 *
 * @invariant Every tree node from em_tree.h has a matching handle.
 * @invariant Handles are returned by reference not index.
 * @invariant Users should never attempt to acquire a handle by index.
 * @invariant When removing a handle, it should always be done in conjugation with it's reference
 *            element.
 * @todo This should be fused with em_ui as otherwise circular dependecy annoyances.
 * */

#pragma once

#ifndef EM_POOL_H
#define EM_POOL_H

#include "em_ui.h"
#include "em_type.h"

#endif
