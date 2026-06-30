#ifndef TUMBLE_JOINTS_EXT_H
#define TUMBLE_JOINTS_EXT_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Extended constraints solved directly against world bodies, outside the core
 * joint list. Each is prepared once per step then iterated. They share the
 * sequential-impulse style used by the contact solver but are kept here so the
 * core stays small. */

typedef enum {
    TMB_XJOINT_WELD,      /* lock relative position and rotation */
    TMB_XJOINT_PRISMATIC, /* slide along a fixed axis, no rotation */
    TMB_XJOINT_MOTOR,     /* drive relative angle toward a target rate */
    TMB_XJOINT_MOUSE,     /* pull one body's anchor toward a world target */
    TMB_XJOINT_PULLEY     /* coupled lengths over two anchors */
} tmb_xjoint_kind;

typedef struct tmb_xjoint {
    tmb_xjoint_kind kind;
    uint32_t a, b;
    tmb_vec2 local_a, local_b;
    tmb_vec2 axis;        /* prismatic / pulley ground anchors */
    tmb_vec2 target;      /* mouse target (world) */
    float ratio;          /* pulley ratio */
    float rest;           /* weld/prismatic reference angle, pulley length */
    float motor_speed;    /* radians/sec */
    float max_force;      /* mouse / motor clamp */
    float frequency;      /* mouse soft-constraint frequency (Hz) */
    float damping;        /* mouse damping ratio */

    /* scratch */
    tmb_vec2 ra, rb;
    tmb_vec2 axis_n, perp_n;
    float mass_axis, mass_perp, mass_ang;
    tmb_vec2 p_impulse;
    float a_impulse;
    float gamma, bias;
} tmb_xjoint;

void tmb_xjoint_weld(tmb_xjoint *j, uint32_t a, uint32_t b);
void tmb_xjoint_prismatic(tmb_xjoint *j, uint32_t a, uint32_t b, tmb_vec2 axis);
void tmb_xjoint_motor(tmb_xjoint *j, uint32_t a, uint32_t b, float speed,
                      float max_force);
void tmb_xjoint_mouse(tmb_xjoint *j, uint32_t b, tmb_vec2 local,
                      tmb_vec2 target);
void tmb_xjoint_pulley(tmb_xjoint *j, uint32_t a, uint32_t b, tmb_vec2 ground_a,
                       tmb_vec2 ground_b, float ratio);

/* Prepare all constraint mass terms from the current body state. */
void tmb_xjoint_prepare(tmb_xjoint *j, tmb_world *w, float dt);
/* One velocity iteration. */
void tmb_xjoint_solve(tmb_xjoint *j, tmb_world *w);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_JOINTS_EXT_H */
