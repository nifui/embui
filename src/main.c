#include <stdint.h>
#include <stdlib.h>

typedef struct t_rect {
    int x, y, width, height;
} t_rect;

typedef struct t_text {
    char* text;
    // reference to a font specified as the id for easier access.
    int font_id;
} t_text;

typedef struct t_vec2 {
    int x, y;
} t_vec2;

typedef struct t_ctx {
    void (*malloc)(size_t size);
    void (*realloc)(void* ptr, size_t size);
} t_ctx;

// Applies to containers so stuff like boxes and whatnot.
typedef struct t_style {
    // Void pointer to allow setting of custom font implementation.
    void* font;
    int (*text_width)(void* font, const char*);
    int (*text_height)(void* font);

} t_style;

typedef struct t_element_base {
    int    id;
    t_rect rect;
} t_element_base;

typedef union {
    t_element_base base;

} t_element;

typedef struct t_pool {
    struct t_pool* next;
} t_pool;

t_pool* t_pool_create(void* addr, uint16_t size, uint8_t object_size) {
    return NULL;
}

int main() {
}
