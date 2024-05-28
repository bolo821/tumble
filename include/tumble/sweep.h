#ifndef TUMBLE_SWEEP_H
#define TUMBLE_SWEEP_H

#include "tumble/body.h"
#include "tumble/aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Swept bounds: the AABB a body occupies over a whole step given its velocity.
 * A broadphase that uses these (rather than the instantaneous AABB) will not
 * miss fast bodies that would otherwise tunnel between steps. */

/* AABB enclosing the body at its current pose and at pose + vel*dt. */
tmb_aabb tmb_swept_aabb(const tmb_body *b, float dt);
/* Conservatively expand `box` by a velocity over dt plus a skin margin. */
tmb_aabb tmb_expand_by_motion(tmb_aabb box, tmb_vec2 vel, float dt,
                              float margin);
/* A scalar measure of how fast a body is moving relative to its own size; a
 * value above ~0.5 suggests it may tunnel and wants continuous collision. */
float tmb_motion_ratio(const tmb_body *b, float dt);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SWEEP_H */
