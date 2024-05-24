#ifndef TUMBLE_SHAPECAST_H
#define TUMBLE_SHAPECAST_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Cast a circle of radius `r` from `origin` along `dir` and find the first body
 * it touches. Unlike a ray, a circle cast has thickness, so it models a
 * projectile or a character probe. Implemented by advancing along the ray and
 * testing the swept circle against each body's expanded shape. */

typedef struct {
    int hit;
    float t;          /* distance traveled to first contact */
    uint32_t body;
    tmb_vec2 point;
    tmb_vec2 normal;
} tmb_casthit;

tmb_casthit tmb_world_circlecast(tmb_world *w, tmb_vec2 origin, tmb_vec2 dir,
                                 float r, float max_dist);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SHAPECAST_H */
