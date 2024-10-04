#ifndef TUMBLE_INTERP_H
#define TUMBLE_INTERP_H

#include "tumble/body.h"
#include "tumble/transform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Render interpolation. With a fixed-timestep solver, the latest physics state
 * lags the wall clock by up to one step; blending the previous and current
 * poses by the leftover-time fraction (alpha) hides that as smooth motion. */

typedef struct {
    tmb_vec2 pos;
    float angle;
} tmb_pose;

tmb_pose tmb_pose_of(const tmb_body *b);
/* Shortest-arc interpolation of angle, linear interpolation of position. */
tmb_pose tmb_pose_lerp(tmb_pose a, tmb_pose b, float alpha);
/* The interpolated world transform for rendering. */
tmb_xform tmb_pose_xform(tmb_pose p);
float tmb_lerp_angle(float a, float b, float t);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_INTERP_H */
