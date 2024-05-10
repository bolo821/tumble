#ifndef TUMBLE_SEGMENT_H
#define TUMBLE_SEGMENT_H

#include "tumble/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Line-segment geometry: intersections, closest points and distances. These
 * underpin raycasts, segment queries and the manifold clipping in narrowphase,
 * and are pure functions with no allocation. */

typedef struct {
    tmb_vec2 a, b;
} tmb_segment;

/* Closest point on segment [a,b] to point p, and the parameter t in [0,1]. */
tmb_vec2 tmb_seg_closest(tmb_vec2 a, tmb_vec2 b, tmb_vec2 p, float *t);
float tmb_seg_point_dist(tmb_vec2 a, tmb_vec2 b, tmb_vec2 p);

/* Intersect two segments. Writes the hit point and the two parameters when they
 * cross; returns 1 on a proper crossing, 0 otherwise. */
int tmb_seg_intersect(tmb_segment s1, tmb_segment s2, tmb_vec2 *hit, float *t1,
                      float *t2);

/* Closest points between two segments and the distance between them. */
float tmb_seg_seg_dist(tmb_segment s1, tmb_segment s2, tmb_vec2 *p1,
                       tmb_vec2 *p2);

/* Ray (origin, unit dir) vs circle; returns the entry distance or -1. */
float tmb_seg_ray_circle(tmb_vec2 origin, tmb_vec2 dir, tmb_vec2 center,
                     float radius);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SEGMENT_H */
