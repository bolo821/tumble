#ifndef TUMBLE_COLOR_H
#define TUMBLE_COLOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RGBA color helpers for the debug renderer and exporters: construction,
 * blending, HSV conversion and a categorical palette so bodies/islands can be
 * tinted distinctly. */

typedef struct {
    float r, g, b, a;
} tmb_col;

tmb_col tmb_col_rgb(float r, float g, float b);
tmb_col tmb_col_rgba(float r, float g, float b, float a);
tmb_col tmb_col_hsv(float h, float s, float v); /* h in [0,360) */
tmb_col tmb_col_lerp(tmb_col a, tmb_col b, float t);
tmb_col tmb_col_scale(tmb_col c, float s);
uint32_t tmb_col_to_rgba8(tmb_col c); /* 0xRRGGBBAA */
tmb_col tmb_col_from_rgba8(uint32_t v);
/* Nth color of an evenly-spaced hue palette. */
tmb_col tmb_palette(int index, int count);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_COLOR_H */
