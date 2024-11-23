#ifndef TUMBLE_BUILDER_H
#define TUMBLE_BUILDER_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Helpers that populate a world with common test arrangements. Each returns the
 * index of the first body it created, or UINT32_MAX on failure. */

uint32_t tmb_build_ground(tmb_world *w, float half_width, float y);
uint32_t tmb_build_box(tmb_world *w, tmb_vec2 pos, float hw, float hh);
uint32_t tmb_build_ball(tmb_world *w, tmb_vec2 pos, float radius);

/* A vertical stack of `count` boxes centered at x, resting from y0 up. */
uint32_t tmb_build_stack(tmb_world *w, float x, float y0, int count, float size);

/* A pyramid `base` boxes wide. */
uint32_t tmb_build_pyramid(tmb_world *w, float y0, int base, float size);

/* A chain of `links` boxes joined end to end, the first pinned to a static
 * anchor at (x,y). */
uint32_t tmb_build_chain(tmb_world *w, float x, float y, int links);

/* A grid of balls joined to their neighbors with distance joints (a cloth). */
uint32_t tmb_build_cloth(tmb_world *w, tmb_vec2 origin, int nx, int ny,
                         float spacing);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_BUILDER_H */
