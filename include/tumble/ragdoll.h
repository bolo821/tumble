#ifndef TUMBLE_RAGDOLL_H
#define TUMBLE_RAGDOLL_H

#include "tumble/world.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Builds an articulated figure -- head, torso, two arms and two legs -- from
 * boxes joined by revolute joints. A compact stress test for the joint solver
 * and a recognizable demo. The returned struct holds the body indices so the
 * caller can poke or query individual limbs. */

typedef struct {
    uint32_t head;
    uint32_t torso;
    uint32_t upper_arm[2];
    uint32_t lower_arm[2];
    uint32_t upper_leg[2];
    uint32_t lower_leg[2];
    int body_count;
} tmb_ragdoll;

/* Spawn a ragdoll with its torso centered at `origin`, scaled by `scale`.
 * Returns 0 on success. */
int tmb_ragdoll_spawn(tmb_world *w, tmb_vec2 origin, float scale,
                      tmb_ragdoll *out);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_RAGDOLL_H */
