#ifndef TUMBLE_FORCES_H
#define TUMBLE_FORCES_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Accumulate forces/impulses on a body. `point` is in world space. */
void tmb_body_apply_force(tmb_body *b, tmb_vec2 force, tmb_vec2 point);
void tmb_body_apply_force_center(tmb_body *b, tmb_vec2 force);
void tmb_body_apply_torque(tmb_body *b, float torque);
void tmb_body_apply_impulse(tmb_body *b, tmb_vec2 impulse, tmb_vec2 point);

/* World-level force fields applied to every dynamic body for this step. */
void tmb_apply_uniform(tmb_world *w, tmb_vec2 accel);
void tmb_apply_drag(tmb_world *w, float linear, float angular);
void tmb_apply_wind(tmb_world *w, tmb_vec2 wind, float coefficient);
void tmb_apply_attractor(tmb_world *w, tmb_vec2 center, float strength,
                         float falloff);
/* A radial impulse from `center`, e.g. an explosion. */
void tmb_apply_explosion(tmb_world *w, tmb_vec2 center, float strength,
                         float radius);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_FORCES_H */
