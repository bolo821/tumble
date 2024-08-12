#ifndef TUMBLE_MOTION_H
#define TUMBLE_MOTION_H

#include "tumble/world.h"
#include "tumble/aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Aggregate dynamics diagnostics over a world, handy for tests and for checking
 * that a simulation is settling (energy decreasing) rather than exploding. */
float tmb_world_kinetic_energy(const tmb_world *w);
tmb_vec2 tmb_world_linear_momentum(const tmb_world *w);
float tmb_world_angular_momentum(const tmb_world *w);
tmb_vec2 tmb_world_center_of_mass(const tmb_world *w);
float tmb_world_total_mass(const tmb_world *w);
tmb_aabb tmb_world_bounds(tmb_world *w);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_MOTION_H */
