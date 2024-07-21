#ifndef TUMBLE_SPRING_H
#define TUMBLE_SPRING_H
#include "tumble/world.h"
#ifdef __cplusplus
extern "C" {
#endif
/* A damped spring (Hooke's law) between anchor points on two bodies, applied as
 * forces. Call once per step before tmb_world_step. */
typedef struct {
    uint32_t a, b;
    tmb_vec2 anchor_a, anchor_b; /* local */
    float rest_length;
    float stiffness;
    float damping;
} tmb_spring;
void tmb_spring_apply(tmb_world *w, const tmb_spring *s);
#ifdef __cplusplus
}
#endif
#endif
