#ifndef TUMBLE_PICK_H
#define TUMBLE_PICK_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Higher-level spatial queries built on the broadphase and shape tests: the k
 * nearest bodies to a point, every body within a radius, and a region census.
 * These are convenience layers over the per-shape predicates for editors and
 * gameplay code. */

/* Bodies whose center lies within `radius` of `p`, written to out (up to cap),
 * unsorted. Returns the count found (which may exceed cap). */
size_t tmb_pick_radius(tmb_world *w, tmb_vec2 p, float radius, uint32_t *out,
                       size_t cap);

/* The k nearest bodies to p, written nearest-first to out (up to k). Returns
 * how many were written. */
size_t tmb_pick_knearest(tmb_world *w, tmb_vec2 p, size_t k, uint32_t *out);

/* Count dynamic and static bodies whose center lies inside `region`. */
void tmb_pick_census(tmb_world *w, tmb_aabb region, size_t *dyn, size_t *stat);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_PICK_H */
