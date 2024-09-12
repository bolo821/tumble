#ifndef TUMBLE_SPLINE_H
#define TUMBLE_SPLINE_H

#include "tumble/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Smooth curves through control points: Catmull-Rom (interpolating) and cubic
 * Bezier. Used to lay out conveyor paths, camera moves and steering waypoint
 * lanes. Sampling is by the curve parameter; arc-length helpers give roughly
 * even spacing. */

/* Catmull-Rom point on the segment between p1 and p2, with neighbors p0,p3. */
tmb_vec2 tmb_catmull_rom(tmb_vec2 p0, tmb_vec2 p1, tmb_vec2 p2, tmb_vec2 p3,
                         float t);
tmb_vec2 tmb_catmull_tangent(tmb_vec2 p0, tmb_vec2 p1, tmb_vec2 p2, tmb_vec2 p3,
                             float t);

/* Sample a Catmull-Rom spline through `n` control points into `out` (samples
 * total, evenly in parameter space). Returns the number written. */
int tmb_spline_sample(const tmb_vec2 *ctrl, int n, tmb_vec2 *out, int samples);

tmb_vec2 tmb_bezier3(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, tmb_vec2 d, float t);
/* Approximate arc length of a cubic Bezier by subdivision. */
float tmb_bezier3_length(tmb_vec2 a, tmb_vec2 b, tmb_vec2 c, tmb_vec2 d,
                         int steps);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SPLINE_H */
