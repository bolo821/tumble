#ifndef TUMBLE_STEERING_H
#define TUMBLE_STEERING_H

#include "tumble/body.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Classic steering behaviors that turn a goal into a desired force on a body:
 * seek, flee, arrive (slow into a target) and path following over a list of
 * waypoints. These drive characters and agents without bypassing the physics
 * solver -- they only add forces. */

typedef struct {
    float max_speed;
    float max_force;
    float arrive_radius; /* begin slowing within this distance */
} tmb_steer;

tmb_steer tmb_steer_make(float max_speed, float max_force);

tmb_vec2 tmb_steer_seek(const tmb_steer *s, const tmb_body *b, tmb_vec2 target);
tmb_vec2 tmb_steer_flee(const tmb_steer *s, const tmb_body *b, tmb_vec2 target);
tmb_vec2 tmb_steer_arrive(const tmb_steer *s, const tmb_body *b,
                          tmb_vec2 target);

/* Follow a polyline of waypoints; advances *wp when the body reaches one.
 * Returns the steering force toward the active waypoint. */
tmb_vec2 tmb_steer_path(const tmb_steer *s, const tmb_body *b,
                        const tmb_vec2 *waypoints, int count, int *wp,
                        float reach);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_STEERING_H */
