/**
 * @file em_event.h
 * @brief Event system for user input and callback dispatch.
 *
 * Defines the event types, input state, callback registry, and event
 * propagation utilities used by the UI system.
 *
 * ## Overview
 *
 * The event system is responsible for:
 * - Representing mouse and keyboard input.
 * - Associating widgets with event callbacks.
 * - Performing hit testing to determine the initial event target.
 * - Propagating events up the widget hierarchy until a handler is found.
 *
 * Mouse events are first resolved to the top-most widget beneath the cursor.
 * If that widget does not handle the event, the event is propagated through
 * its ancestors until either:
 *
 * - a callback handles the event, or
 * - the root widget is reached.
 *
 * Callback state is stored separately from the callback itself, allowing the
 * same callback implementation to be reused by multiple widgets with different
 * state objects.
 *
 * @section event_registration Registering Callbacks
 *
 * Each callback is registered with:
 * - the widget handle it belongs to,
 * - a callback function, and
 * - an opaque state pointer.
 *
 * The state pointer is passed unchanged to the callback whenever the event is
 * dispatched. The library never interprets or owns this pointer.
 *
 * Typical usage:
 * @code
 * MyWidgetState state = { ... };
 *
 * em_registry_value value = {
 *     .state = &state,
 *     .handle_idx = widget,
 *     .callback = my_callback,
 * };
 *
 * em_register_callback(ctx, &registry, &value);
 * @endcode
 *
 * @note The library does not allocate, copy, or free callback state. Lifetime
 * management remains the responsibility of the application.
 *
 * @invariant Each registered callback is associated with exactly one widget.
 * @invariant Callback state is represented by an opaque `void *`.
 * @invariant The callback registry does not take ownership of the state
 * pointer.
 */

#pragma once
#ifndef EM_EVENTS_H
#define EM_EVENTS_H
#include <stddef.h>
#include "em_elements.h"
#include "em_type.h"
#include "em_tree.h"

/**
 * @brief Types of input events.
 */
typedef enum em_event_type {
    /** Mouse input. */
    MOUSE,
    /** Keyboard input. */
    KEYBOARD
} em_event_type;

/**
 * @brief Mouse button bit flags.
 */
enum {
    /** Left mouse button. */
    LCLCK = 1 << 0,

    /** Right mouse button. */
    RCLCK = 1 << 1,

    /** Middle mouse button. */
    MCLCK = 1 << 2,

    /** Mouse button 4. */
    M4 = 1 << 3,

    /** Mouse button 5. */
    M5 = 1 << 4,
};

/**
 * @brief Keyboard modifier bit flags.
 */
enum {
    /** Alt key. */
    ALT = 1 << 0,

    /** Control key. */
    CTRL = 1 << 1,

    /** Windows/Super key. */
    WIN = 1 << 2,

    /** Shift key. */
    SHFT = 1 << 3,
};

/**
 * @brief Mouse event data.
 */
typedef struct em_mouse_event {
    /** Cursor position in UI coordinates. */
    em_vec2 position;

    /** Combination of mouse button flags. */
    int mouse_flags;
} em_mouse_event;

/**
 * @brief Keyboard event data.
 */
typedef struct em_keyboard_event {
    /** Platform-specific key code. */
    int keycode;

    /** Combination of modifier flags. */
    int modifier;
} em_keyboard_event;

/**
 * @brief Generic UI event.
 *
 * The active member of the union is determined by ::type.
 *
 * @invariant Event generation is not handled by the library and the user must create it.
 */
typedef struct em_event {
    /** Event category. */
    em_event_type type;

    union {
        /** Mouse event payload. */
        em_mouse_event mouse;

        /** Keyboard event payload. */
        em_keyboard_event keyboard;
    };
} em_event;

/**
 * @brief Callback invoked when an event is dispatched.
 *
 * @param state User-defined state pointer supplied during registration.
 * @param event Event being delivered.
 */
typedef void (*em_callback)(void* state, em_event event);

/**
 */
typedef struct em_handler {
    /** User-defined callback state. */
    void* state;

    /** Callback invoked for dispatched events. */
    em_callback callback;

    uint8_t ref_count; //**< How many resources reference this item */

} em_handler;

/**
 * @brief Registry containing all registered callbacks.
 */
/** Dynamic array of callback registrations. */
typedef EM_VECTOR(em_handler, em_handlers);

/**
 * @brief Removes the callback associated with a widget.
 *
 * @param registry Callback registry.
 * @param handle_idx Widget whose callback should be removed
 *
 * @retval EM_OK Callback removed.
 * @retval EM_ERR_INVALID_HANDLE No callback exists for the handle.
 */
em_result em_remove_handler(em_handlers* handlers, em_idx handle_idx);

/**
 * @brief Registers a callback which makes it available for a widget to link to.
 *
 * @param ctx Library context.
 * @param registry Callback registry.
 * @param value Callback registration information.
 * @param[out] dst_idx Index of where the callback was registered.
 *
 * @retval EM_OK Registration succeeded.
 * @retval EM_ERR_OUT_OF_MEMORY Registry growth failed.
 */
em_result em_add_handler(em_ctx* ctx, em_handler* handler, em_handlers* handlers, em_idx* dst_idx);

/**
 * @brief Dispatches an event through the widget hierarchy.
 *
 * Event propagation begins at the specified widget and continues toward the
 * root until a suitable callback is found or the root is reached.
 *
 * @param ui UI context.
 * @param registry Callback registry.
 * @param event Event to dispatch.
 * @param initial Initial widget that receives the event.
 *
 * @retval EM_OK Event propagation completed.
 * @retval EM_ERR_CAPACITY The parent collection array did not have enough capacity.
 */
em_result em_propogate_event(em_ctx*      ui,
                             em_tree*     tree,
                             em_handlers* handlers,
                             em_event*    event,
                             em_idx       initial);

/**
 * @brief Performs hit testing to determine the target widget.
 *
 * Finds the widget that should initially receive a pointer event based on the
 * current cursor position.
 *
 * @return Result code indicating success or failure.
 *
 */

// Unsure of how to actually do this, as a command-based approach isn't too hard (requires keeping
// handle index or some other metadata) However if the user opts for a func pointer approach where
// for each call a function pointer is called, its a lot harder as the data does not exist to check.
// The layout also would have to be recalculated and requires persistent storage which would make
// storage annoying. ANother possible solution would be to have another struct dedicated
// specifically for UI layout. It encodes the bounding box of the outputted elements in the tree
// which can be used for hit testing.

// If we encode the layout relationship its basically just O(tree_depth) + some layout calculations.
// But this assumes that no children elements overlap between two parents. The fix for this would be
// working from the bottom up but that ends up forcing a whole recalculation of the tree's layout if
// we don't have it prior. A parallel layout tree could be created at the cost of more memory. I
// don't think there's a good workaround having a persistent layout tree whether it be through
// commands or another struct.
em_result em_find_target();

#endif
