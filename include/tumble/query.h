#ifndef TUMBLE_QUERY_H
#define TUMBLE_QUERY_H

#include "tumble/world.h"
#include "tumble/aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Is the world point p inside the body's shape? */
int tmb_body_contains(const tmb_body *b, tmb_vec2 p);

/* The first body whose shape contains p, or UINT32_MAX. */
uint32_t tmb_world_query_point(tmb_world *w, tmb_vec2 p);

/* Indices of bodies whose AABB overlaps `box`, into out (up to cap). */
size_t tmb_world_query_aabb(tmb_world *w, tmb_aabb box, uint32_t *out,
                            size_t cap);

/* The body whose center is nearest to p, or UINT32_MAX if the world is empty. */
uint32_t tmb_world_nearest(tmb_world *w, tmb_vec2 p);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_QUERY_H */
