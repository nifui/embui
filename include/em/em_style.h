#pragma once
#ifndef EM_STYLE_H
#define EM_STYLE_H

#include "em_elements.h"
#include "em_type.h"

/**
 * @brief Color representation
 */
typedef struct em_color {
    uint8_t r, g, b, a;
} em_color;

/**
 * @brief Enum describing the direction of which layouts should be calculated
 */
typedef enum em_direction {
    HORIZONTAL, //**< Lay elements out in a column */
    VERTICAL,   //**< Lay elements out in a row */
    NONE,       //**< Let the library decide or do nothing. */
} em_direction;

/**
 * @brief Describes how elements should be positioned relative to other elements near it
 *
 * Affects only it's siblings and children. By default the children attempt to inherit certain
 * properties if applicable.
 *
 * @note Inheritance might be horrible idea for this but there has to be some way of preventing the
 * user having to set the style of every element.
 */
typedef struct em_layout {
    em_direction direction;   //**< Direction to place elements */
    char*        constraints; //**< List of numbers to constrain the layout of elements by. */
    char         margin;      //**< How much margin should be between sibings */
    char         padding;     //**<  */

} em_layout;

#define EM_LAYOUT_DEFAULT (em_layout){.direction = 0}

/**
 * @brief Styling for primitives.
 *
 * @todo Add animation support.
 * @note To calculate the animation frames needed, collect the referenced styles.
 */

typedef struct em_style {
    em_layout layout;
    em_color  text_color;
    em_color  bg_color;
    em_color  border_color;
    char      rounding;
    em_idx    next_frame;

} em_style;

typedef EM_VECTOR(em_style, em_styles);

#define EM_STYLE_DEFAULT (em_style){.layout = 0}

/**
 * @brief Add a style and return a handle to it for the user to hold.
 *
 * @param style The style to insert.
 * @param[out] handle The output parameter to hold the handle.
 *
 * @em_result Status of adding the style.
 */
em_result em_add_style(em_ctx* ctx, em_styles* styles, em_style style, em_handle* handle);

/**
 * @brief Removes a style
 *
 * @param handle The handle at which to remove the style.
 *
 * @pre Prior to running this ensure that the style's ref_count > 0.
 *
 * @return Return value description
 */
em_result em_remove_style(em_ctx* ctx, em_styles* styles, em_handle* handle);

/**
 * @brief Allows modifying a specific style.
 *
 * @param parameter Description of parameter.
 *
 * @return Return value description
 */
em_result em_modify_style(em_ctx* ctx, em_styles* styles, em_handle* handle, em_style* style_ptr);

/**
 * @brief Function description
 *
 * @param parameter Description of parameter.
 *
 * @return Return value description
 */
em_result em_copy_style(em_ctx* ctx, em_styles* styles, em_handle* handle);

#endif
