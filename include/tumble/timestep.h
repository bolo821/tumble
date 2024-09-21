#ifndef TUMBLE_TIMESTEP_H
#define TUMBLE_TIMESTEP_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A fixed-timestep accumulator. Real frames arrive at a variable rate, but a
 * stable solver wants a constant dt; this banks elapsed time and runs whole
 * fixed steps, exposing an interpolation alpha for smooth rendering between
 * them. */

typedef struct {
    float fixed_dt;
    float accumulator;
    float alpha;     /* interpolation factor in [0,1] after advance */
    int max_steps;   /* clamp to avoid the spiral of death */
    uint64_t total_steps;
} tmb_timestep;

void tmb_timestep_init(tmb_timestep *ts, float fixed_dt);
/* Bank `frame_dt` and run as many fixed steps as fit. Returns the number of
 * steps actually run. */
int tmb_timestep_advance(tmb_timestep *ts, tmb_world *w, float frame_dt);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_TIMESTEP_H */
