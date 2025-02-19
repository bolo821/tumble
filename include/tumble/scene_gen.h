#ifndef TUMBLE_SCENE_GEN_H
#define TUMBLE_SCENE_GEN_H

#include "tumble/world.h"
#include "tumble/rng.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Procedural scene generators: deterministic given a seed, they populate a
 * world with arrangements useful for stress tests and demos. Each returns the
 * number of bodies it added. */

size_t tmb_gen_brick_wall(tmb_world *w, tmb_vec2 origin, int cols, int rows,
                          float bw, float bh);
size_t tmb_gen_ball_pit(tmb_world *w, tmb_rng *rng, tmb_aabb region, int count,
                        float min_r, float max_r);
size_t tmb_gen_domino_line(tmb_world *w, tmb_vec2 start, int count,
                           float spacing);
size_t tmb_gen_scatter_polygons(tmb_world *w, tmb_rng *rng, tmb_aabb region,
                                 int count, int min_sides, int max_sides);
/* A static terrain chain from a heightfield sampled across [x0,x1]. */
size_t tmb_gen_terrain(tmb_world *w, float x0, float x1, int segments,
                       const float *heights);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SCENE_GEN_H */
