#pragma once
#ifndef EM_EVENTS_H
#define EM_EVENTS_H

#define MAX_UI_DEPTH 16
#include "em_ui.h"
// A touch event could just be interpreted as a mouse click.
typedef enum em_event_type { MOUSE, KEYBOARD } em_event_type;

enum {
    LCLCK = 1 << 0,
    RCLCK = 1 << 1,
    MCLCK = 1 << 2,
    M4    = 1 << 3,
    M5    = 1 << 4,
};
enum {
    ALT  = 1 << 0,
    CTRL = 1 << 1,
    WIN  = 1 << 2,
    SHFT = 1 << 3,
};

typedef struct em_mouse_event {
    em_vec2 position;
    int     mouse_flags;
} em_mouse_event;

typedef struct em_keyboard_event {
    int keycode;
    int modifier;
} em_keyboard_event;

typedef struct em_event {
    em_event_type type;
    union {
        em_mouse_event    mouse;
        em_keyboard_event keyboard;
    };
} em_event;

typedef void (*em_callback)(void *state, em_event event);

typedef struct {
    void       *state;
    em_idx      handle_idx;
    em_callback callback;
} em_registry_value;

typedef struct {
    EM_VECTOR(em_registry_value, values);
} em_callback_registry;

int em_deregister_value(em_callback_registry *registry, em_idx handle_idx);

int em_register_value(em_ctx *ctx, em_callback_registry *registry, em_registry_value *value);
int em_propogate_event(em_ctx *ui, em_callback_registry *registry, em_event *event, em_idx initial);
// Traverses the command list and tries to find a valid target for an event.
int em_find_target();
#endif
