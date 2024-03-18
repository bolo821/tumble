#ifndef TUMBLE_BODY_H
#define TUMBLE_BODY_H

#include "tumble/shape.h"
#include "tumble/transform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TMB_BODY_STATIC,  /* infinite mass, never moves */
    TMB_BODY_DYNAMIC
} tmb_body_type;

/* A rigid body: state, material and a single collision shape. */
typedef struct {
    tmb_body_type type;
    tmb_vec2 pos;
    float angle;
    tmb_vec2 vel;
    float ang_vel;
    tmb_vec2 force;
    float torque;
    float density;
    float mass, inv_mass;
    float inertia, inv_inertia;
    float restitution; /* bounciness, 0..1 */
    float friction;
    tmb_shape shape;
    tmb_xform xf; /* cached world transform; refresh with tmb_body_sync */
} tmb_body;

void tmb_body_init(tmb_body *b, tmb_body_type type, tmb_shape shape,
                   tmb_vec2 pos);
/* Recompute mass and inertia from the shape and density (dynamic bodies). */
void tmb_body_set_density(tmb_body *b, float density);
/* Refresh the cached transform from pos/angle. */
void tmb_body_sync(tmb_body *b);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_BODY_H */
