#ifndef TUMBLE_BUOYANCY_H
#define TUMBLE_BUOYANCY_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A half-plane of fluid (water below a horizontal surface) that applies
 * buoyancy and linear drag to any body dipping into it. The submerged area is
 * approximated from the body's AABB clipped to the surface -- cheap and stable
 * for gameplay water. Apply once per step before integration. */

typedef struct {
    float surface_y;   /* world y of the fluid surface */
    float density;     /* fluid density */
    tmb_vec2 gravity;  /* gravity used for the buoyant force */
    float linear_drag; /* velocity damping while submerged */
    tmb_vec2 current;  /* fluid flow velocity */
} tmb_fluid;

tmb_fluid tmb_fluid_make(float surface_y, float density, tmb_vec2 gravity);

/* Fraction of a body's AABB height below the surface, in [0,1]. */
float tmb_fluid_submersion(const tmb_fluid *f, tmb_body *b);

/* Apply buoyancy and drag forces to every dynamic body in the world. Returns
 * the number of bodies that were at least partly submerged. */
size_t tmb_fluid_apply(const tmb_fluid *f, tmb_world *w);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_BUOYANCY_H */
