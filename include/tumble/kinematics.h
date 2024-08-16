#ifndef TUMBLE_KINEMATICS_H
#define TUMBLE_KINEMATICS_H

#include "tumble/body.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Convenience operations on a body's motion state: applying forces and
 * impulses at a point, reading the velocity of a point on the body, and energy
 * accounting. These keep gameplay code from poking the raw fields and getting
 * the lever-arm terms wrong. */

/* World velocity of the world-space point `p` on the body (v + w x r). */
tmb_vec2 tmb_point_velocity(const tmb_body *b, tmb_vec2 p);

/* Accumulate a force/impulse applied at a world point (adds the torque term). */
void tmb_apply_force_at(tmb_body *b, tmb_vec2 force, tmb_vec2 point);
void tmb_apply_impulse_at(tmb_body *b, tmb_vec2 impulse, tmb_vec2 point);
void tmb_apply_torque(tmb_body *b, float torque);

/* Set linear/angular velocity so the body reaches `target` in `dt` seconds. */
void tmb_drive_to(tmb_body *b, tmb_vec2 target, float dt);

float tmb_kinetic_energy(const tmb_body *b);
tmb_vec2 tmb_linear_momentum(const tmb_body *b);
float tmb_angular_momentum(const tmb_body *b);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_KINEMATICS_H */
