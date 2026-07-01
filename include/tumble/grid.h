#ifndef TUMBLE_GRID_H
#define TUMBLE_GRID_H

#include "tumble/world.h"
#include "tumble/aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A uniform spatial-hash grid over the bodies of a world, for accelerating
 * region and point queries. Snapshots body AABBs at build time. */
typedef struct tmb_grid tmb_grid;

tmb_grid *tmb_grid_build(tmb_world *w, float cell);
void tmb_grid_free(tmb_grid *g);

/* Write the indices of bodies whose AABB overlaps `box` into out (up to cap);
 * returns the count (which may exceed cap). */
size_t tmb_grid_query_aabb(const tmb_grid *g, tmb_aabb box, uint32_t *out,
                           size_t cap);
size_t tmb_grid_query_point(const tmb_grid *g, tmb_vec2 p, uint32_t *out,
                            size_t cap);

/* Fill a `side` x `side` occupancy heatmap: counts[y*side + x] receives the
 * number of bodies whose grid cell folds into bucket (x, y). `counts` must
 * hold at least side*side ints. */
void tmb_grid_occupancy(const tmb_grid *g, int *counts, int side);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_GRID_H */
