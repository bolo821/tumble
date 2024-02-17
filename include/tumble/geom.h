#ifndef TUMBLE_GEOM_H
#define TUMBLE_GEOM_H

#include "tumble/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Planar geometry predicates and constructions used throughout the engine:
 * orientation tests, barycentric coordinates, circle fits and projections.
 * Pure functions, no allocation. */

/* >0 if c is left of the directed line a->b, <0 right, 0 collinear. */
float tmb_orient2d(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c);
int tmb_segments_cross(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, tmb_vec2 d);

/* Barycentric coordinates of p w.r.t. triangle (a,b,c). */
void tmb_barycentric(tmb_vec2 p, tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, float *u,
                     float *v, float *w);

/* Circumcircle of a triangle; returns 0 on success (degenerate -> nonzero). */
int tmb_circumcircle(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, tmb_vec2 *center,
                     float *radius);

/* Project p onto the infinite line through a with direction dir. */
tmb_vec2 tmb_project_point_line(tmb_vec2 p, tmb_vec2 a, tmb_vec2 dir);
/* Reflect v about a unit normal n. */
tmb_vec2 tmb_reflect(tmb_vec2 v, tmb_vec2 n);
/* Signed angle from a to b in radians, in (-pi, pi]. */
float tmb_angle_between(tmb_vec2 a, tmb_vec2 b);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_GEOM_H */
