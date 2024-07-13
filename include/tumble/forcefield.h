#ifndef TUMBLE_FORCEFIELD_H
#define TUMBLE_FORCEFIELD_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Region force fields: radial attractors/repellers, vortices and uniform
 * pushes. Each has a center, a radius of influence and a falloff, and adds a
 * force to dynamic bodies whose center lies within range. Combine several for
 * tractor beams, explosions and whirlpools. */

typedef enum {
    TMB_FIELD_RADIAL, /* push toward/away from center */
    TMB_FIELD_VORTEX, /* tangential swirl about center */
    TMB_FIELD_UNIFORM /* constant direction within the radius */
} tmb_field_kind;

typedef enum {
    TMB_FALLOFF_NONE,    /* constant within radius */
    TMB_FALLOFF_LINEAR,  /* 1 - d/r */
    TMB_FALLOFF_INVERSE  /* ~ 1/d^2, clamped near the center */
} tmb_field_falloff;

typedef struct {
    tmb_field_kind kind;
    tmb_field_falloff falloff;
    tmb_vec2 center;
    tmb_vec2 direction; /* for uniform fields */
    float radius;
    float strength; /* positive attracts / +swirl, negative repels */
} tmb_field;

tmb_field tmb_field_attractor(tmb_vec2 center, float radius, float strength);
tmb_field tmb_field_vortex(tmb_vec2 center, float radius, float strength);
tmb_field tmb_field_uniform(tmb_vec2 center, float radius, tmb_vec2 dir,
                            float strength);

float tmb_field_weight(const tmb_field *f, float dist);
tmb_vec2 tmb_field_force_at(const tmb_field *f, tmb_vec2 p);
/* Apply the field to all dynamic bodies; returns the count affected. */
size_t tmb_field_apply(const tmb_field *f, tmb_world *w);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_FORCEFIELD_H */
