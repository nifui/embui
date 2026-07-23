/*
 * Defines templates for common widgets like buttons, textboxes, and whatnot. No styling by default.
 **/
#pragma once
#ifndef EM_TEMPLATES_H

#include "em_ui.h"

// Instead of returning a subtree, we can just attach directly to the nodes but it removes the
// ergonomic aspect of the whole UI.
int em_add_buton(em_idx target_idx) {
}

#define EM_TEMPLATES_H
#endif
