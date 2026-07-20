#pragma once

#define EM_ENABLE_INT
#define EM_ENABLE_FLOAT
#define EM_ENABLE_BEZIER_INT
#define EM_ENABLE_BEZIER_FLOAT
#include "em_math.h"

typedef struct test_vec2 {
    int x, y;
} test_vec2;

void draw_rect(em_rectf rect) {}

void draw_line(em_linei line) {
    int x1              = line.p0.x;
    int y1              = line.p0.y;
    int m_new           = 2 * (line.p1.y - line.p0.y);
    int slope_error_new = m_new - (line.p1.x - line.p0.x);
    for (int x = line.p0.x, y = line.p0.y; x <= line.p1.x; x++) {
        slope_error_new += m_new;
        if (slope_error_new >= 0) {
            y++;
            slope_error_new -= 2 * (line.p1.x - line.p0.x);
        }
    }
}

#define DRAW_PIXEL(x, y, brightness)
#define BRIGHTNESS_RES 8
#define FP_FRAC_BITS 15
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
