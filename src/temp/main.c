#include <GLFW/glfw3.h>
#include <stdint.h>
#include "em_math.h"
#include <string.h>
#include <stdio.h>
#define EM_ENABLE_INT
#define EM_ENABLE_FLOAT
#define EM_ENABLE_BEZIER_INT
#define EM_ENABLE_BEZIER_FLOAT
#define WIDTH 480
#define HEIGHT 320

static uint32_t framebuffer[WIDTH * HEIGHT];

#define DRAW_PIXEL(x, y, brightness)                                                               \
    do {                                                                                           \
        uint32_t* p   = &framebuffer[(y) * WIDTH + (x)];                                           \
        uint32_t  old = *p & 0xff;                                                                 \
        uint32_t  b   = old + (brightness);                                                        \
        if (b > 255)                                                                               \
            b = 255;                                                                               \
        *p = (255u << 24) | (b << 16) | (b << 8) | b;                                              \
    } while (0)
#define BRIGHTNESS_RES 1
#define FP_FRAC_BITS 7
#define MAX_BRIGHTNESS 255

#define FP_ONE (1 << FP_FRAC_BITS)
#define FP_MASK (FP_ONE - 1)

static inline int ipart(int fp) {
    return fp >> FP_FRAC_BITS;
}

static inline int fpart(int fp) {
    return fp & FP_MASK;
}

static inline int rfpart(int fp) {
    return FP_ONE - fpart(fp);
}

static inline uint8_t fp_to_brightness(int fp) {
    return (uint8_t)((fp * MAX_BRIGHTNESS) >> FP_FRAC_BITS);
}

void draw_line_xiaolin(em_linei line) {
    int x0 = line.p0.x;
    int y0 = line.p0.y;
    int x1 = line.p1.x;
    int y1 = line.p1.y;

    int steep = (y1 > y0 ? y1 - y0 : y0 - y1) > (x1 > x0 ? x1 - x0 : x0 - x1);

    if (steep) {
        int t;

        t  = x0;
        x0 = y0;
        y0 = t;
        t  = x1;
        x1 = y1;
        y1 = t;
    }

    if (x0 > x1) {
        int t;

        t  = x0;
        x0 = x1;
        x1 = t;
        t  = y0;
        y0 = y1;
        y1 = t;
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    if (dx == 0) {
        /* Vertical line */
        if (y0 > y1) {
            int t = y0;
            y0    = y1;
            y1    = t;
        }

        for (int y = y0; y <= y1; y++) {
            if (steep)
                DRAW_PIXEL(y, x0, MAX_BRIGHTNESS);
            else
                DRAW_PIXEL(x0, y, MAX_BRIGHTNESS);
        }
        return;
    }

    /* Fixed-point gradient */
    int gradient = (dy << FP_FRAC_BITS) / dx;

    int intery = (y0 << FP_FRAC_BITS);

    for (int x = x0; x <= x1; x++) {

        int y = ipart(intery);

        uint8_t upper = fp_to_brightness(rfpart(intery));
        uint8_t lower = fp_to_brightness(fpart(intery));

        if (steep) {
            DRAW_PIXEL(y, x, upper);
            DRAW_PIXEL(y + 1, x, lower);
        } else {
            DRAW_PIXEL(x, y, upper);
            DRAW_PIXEL(x, y + 1, lower);
        }

        intery += gradient;
    }
}

void clear_framebuffer(void) {
    memset(framebuffer, 0, sizeof(framebuffer));
}

void draw_radial(void) {
    em_vec2i c = {150, 150};

    draw_line_xiaolin((em_linei){c, {250, 150}});
    draw_line_xiaolin((em_linei){c, {230, 80}});
    draw_line_xiaolin((em_linei){c, {150, 50}});
    draw_line_xiaolin((em_linei){c, {70, 80}});
    draw_line_xiaolin((em_linei){c, {50, 150}});
    draw_line_xiaolin((em_linei){c, {70, 220}});
    draw_line_xiaolin((em_linei){c, {150, 250}});
    draw_line_xiaolin((em_linei){c, {230, 220}});
}

void draw_house(void) {
    draw_line_xiaolin((em_linei){{100, 150}, {200, 150}});
    draw_line_xiaolin((em_linei){{200, 150}, {200, 250}});
    draw_line_xiaolin((em_linei){{200, 250}, {100, 250}});
    draw_line_xiaolin((em_linei){{100, 250}, {100, 150}});
    draw_line_xiaolin((em_linei){{100, 150}, {150, 80}});
    draw_line_xiaolin((em_linei){{150, 80}, {200, 150}});
}

void draw_star(void) {
    draw_line_xiaolin((em_linei){{150, 50}, {180, 140}});
    draw_line_xiaolin((em_linei){{180, 140}, {280, 140}});
    draw_line_xiaolin((em_linei){{280, 140}, {200, 200}});
    draw_line_xiaolin((em_linei){{200, 200}, {230, 300}});
    draw_line_xiaolin((em_linei){{230, 300}, {150, 240}});
    draw_line_xiaolin((em_linei){{150, 240}, {70, 300}});
    draw_line_xiaolin((em_linei){{70, 300}, {100, 200}});
    draw_line_xiaolin((em_linei){{100, 200}, {20, 140}});
    draw_line_xiaolin((em_linei){{20, 140}, {120, 140}});
    draw_line_xiaolin((em_linei){{120, 140}, {150, 50}});
}

void draw_rectangle(void) {
    draw_line_xiaolin((em_linei){{50, 50}, {250, 50}});
    draw_line_xiaolin((em_linei){{250, 50}, {250, 150}});
    draw_line_xiaolin((em_linei){{250, 150}, {50, 150}});
    draw_line_xiaolin((em_linei){{50, 150}, {50, 50}});
}

void draw_frame(void) {
    draw_radial();
}

int main(void) {
    if (!glfwInit())
        return 1;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CPU Rasterizer", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // disable VSync

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    while (!glfwWindowShouldClose(window)) {
        // double start = glfwGetTime();
        draw_frame();
        // double rasterTime = glfwGetTime() - start;
        // printf("Raster: %.3f ms (%.1f FPS)\n", rasterTime * 1000.0, 1.0 / rasterTime);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteTextures(1, &texture);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
