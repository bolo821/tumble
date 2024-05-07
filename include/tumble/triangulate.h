#ifndef TUMBLE_TRIANGULATE_H
#define TUMBLE_TRIANGULATE_H

#include "tumble/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Ear-clipping triangulation of a simple polygon. The output is a flat list of
 * vertex-index triples; with it a concave outline can be decomposed into convex
 * pieces (e.g. to feed several convex shapes into the collision system) or
 * measured/meshed. */

typedef struct {
    int a, b, c;
} tmb_tri;

/* Triangulate the simple polygon `pts` (n vertices). Writes up to `cap`
 * triangles to `out`; returns the triangle count (n-2 on success, 0 on
 * failure). */
int tmb_triangulate(const tmb_vec2 *pts, int n, tmb_tri *out, int cap);

/* Sum of triangle areas (a robust polygon area for non-convex shapes). */
float tmb_triangulated_area(const tmb_vec2 *pts, int n, const tmb_tri *tris,
                            int count);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_TRIANGULATE_H */
