#ifndef TUMBLE_HULL_H
#define TUMBLE_HULL_H

#include "tumble/vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Planar polygon utilities: convex hull, signed area, centroid and winding.
 * The hull (Andrew's monotone chain) is what turns an arbitrary point cloud
 * into a valid convex shape for the collision system. */

/* Compute the convex hull of `n` input points. Writes up to `cap` hull vertices
 * in counter-clockwise order to `out`; returns the hull vertex count. */
int tmb_convex_hull_capped(const tmb_vec2 *pts, int n, tmb_vec2 *out, int cap);

/* Signed area (>0 for counter-clockwise winding). */
float tmb_polygon_area(const tmb_vec2 *pts, int n);
tmb_vec2 tmb_polygon_centroid(const tmb_vec2 *pts, int n);
int tmb_polygon_is_convex(const tmb_vec2 *pts, int n);
int tmb_polygon_contains(const tmb_vec2 *pts, int n, tmb_vec2 p);
/* Reverse winding in place. */
void tmb_polygon_reverse(tmb_vec2 *pts, int n);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_HULL_H */
