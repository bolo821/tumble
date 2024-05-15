#ifndef TUMBLE_DISTANCE_H
#define TUMBLE_DISTANCE_H

#include "tumble/shape.h"
#include "tumble/transform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The world support point of a shape in direction d (farthest point along d). */
tmb_vec2 tmb_shape_support_world(const tmb_shape *s, tmb_xform xf, tmb_vec2 d);

/* Separation distance between two convex shapes via GJK. Returns 0 if they
 * overlap. The closest points on each shape are written through pa/pb when
 * non-NULL (and the shapes are separated). */
float tmb_shape_distance(const tmb_shape *sa, tmb_xform xa, const tmb_shape *sb,
                         tmb_xform xb, tmb_vec2 *pa, tmb_vec2 *pb);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_DISTANCE_H */
