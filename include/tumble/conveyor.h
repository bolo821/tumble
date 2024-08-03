#ifndef TUMBLE_CONVEYOR_H
#define TUMBLE_CONVEYOR_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A conveyor belt: a rectangular region with a surface velocity. Dynamic
 * bodies overlapping it are nudged toward the belt speed along the belt
 * direction, modelling a moving floor without a kinematic body type. */

typedef struct {
    tmb_aabb region;
    tmb_vec2 direction; /* unit belt direction */
    float speed;        /* surface speed along direction */
    float grip;         /* 0..1 how strongly the belt drags bodies */
} tmb_conveyor;

tmb_conveyor tmb_conveyor_make(tmb_aabb region, tmb_vec2 direction,
                               float speed);
/* Apply the belt to overlapping dynamic bodies; returns the count moved. */
size_t tmb_conveyor_apply(const tmb_conveyor *c, tmb_world *w, float dt);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_CONVEYOR_H */
