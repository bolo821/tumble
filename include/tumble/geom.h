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

/* Closest point to p on segment [a,b]. When t is non-NULL it receives the
 * parameter in [0,1] such that the result is a + t*(b-a). */
tmb_vec2 tmb_closest_point_on_segment(tmb_vec2 p, tmb_vec2 a, tmb_vec2 b,
                                      float *t);

/* Feature of a triangle nearest a query point (used by GJK simplex
 * reduction). */
typedef enum {
    TMB_TRI_VERT_A = 0,
    TMB_TRI_VERT_B,
    TMB_TRI_VERT_C,
    TMB_TRI_EDGE_AB,
    TMB_TRI_EDGE_BC,
    TMB_TRI_EDGE_CA,
    TMB_TRI_INSIDE
} tmb_tri_region;

/* Closest point to p on triangle (a,b,c). When region is non-NULL it receives
 * the Voronoi feature the closest point lies on; TMB_TRI_INSIDE means p is
 * inside the triangle (closest point == p). */
tmb_vec2 tmb_closest_point_on_triangle(tmb_vec2 p, tmb_vec2 a, tmb_vec2 b,
                                       tmb_vec2 c, tmb_tri_region *region);

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
