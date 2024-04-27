#ifndef TUMBLE_POLYGON_H
#define TUMBLE_POLYGON_H

#include "tumble/vec2.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Geometry utilities on a 2D polygon given as an array of vertices. */
float tmb_poly_signed_area(const tmb_vec2 *v, int n);
float tmb_poly_perimeter(const tmb_vec2 *v, int n);
tmb_vec2 tmb_poly_centroid(const tmb_vec2 *v, int n);
int tmb_poly_is_convex(const tmb_vec2 *v, int n);
int tmb_poly_contains(const tmb_vec2 *v, int n, tmb_vec2 p);

/* Clip a convex polygon against the half-plane dot(normal, x) <= offset,
 * writing the result to out (capacity >= n + 1). Returns the new vertex count. */
int tmb_poly_clip(const tmb_vec2 *v, int n, tmb_vec2 normal, float offset,
                  tmb_vec2 *out);

/* 2D convex hull (monotone chain) of `n` points into hull (capacity >= n);
 * returns the hull vertex count, CCW. */
int tmb_convex_hull(const tmb_vec2 *pts, int n, tmb_vec2 *hull);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_POLYGON_H */
