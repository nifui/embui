#include <stdlib.h>

// Layout caching
// Retained render caching
// Allow trivial state management
//

typedef struct t_rect {
    int x, y, width, height;
} t_rect;

typedef struct t_vec2 {
    int x, y;
} t_vec2;

// Stack based approach where each element has its own depth value allowing for somewhat recursive
// tree without the recursive part?

// Use of a somewhat binary tree?
// For each depth value of the same type increment a depth counter for indexing into the style array
// for mem contiguity? Eg check the depth of the element and use that as the start. After increment
// by 1 to allow processing of the next element. Doesn't work well as this assumes too much stuff
// that overall makes the lib hard to use.
// Ex :
// window
// ├── panel
// │    ├── text
// │    └── button
// └── panel
// would be
// index   depth   type
//--------------------
// 0       0       window
// 1       1       panel
// 2       2       text
// 3       2       button
// 4       1       panel
// If an element is on top of two elements then what could be done is have said element reference
// the two elements it's on. Requires an ID system to properly implement.
typedef enum {
    CMD_RECT,
    CMD_TEXT,
    CMD_IMAGE,
} t_cmd_type;

typedef struct t_cmd_base {

} t_cmd_base;

typedef struct t_rect_cmd {

} t_rect_cmd;

typedef struct t_text_cmd {
} t_text_cmd;

typedef struct {
    t_cmd_type type;

    union {
        t_rect_cmd rect;
        t_text_cmd text;
    };
} t_cmd;

typedef struct t_style {

} t_style;

// Styles is always a multiple of 2^n - 1; Might not be ideal as this can probably lead to
// fragmentation in some cases of overlapping elements. Might be a data structure for this type of
// thing.
typedef struct t_style_stack {
    t_style* styles;
    // Represents node depth
    int current_depth;
} t_style_stack;

int main() {
}
