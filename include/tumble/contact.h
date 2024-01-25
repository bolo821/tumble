#ifndef TUMBLE_CONTACT_H
#define TUMBLE_CONTACT_H

#include "tumble/vec2.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TMB_NO_SIBLING 0xffffffffu

/* One contact constraint (one point of a manifold). A polygon-polygon manifold
 * has up to two points, stored as two contacts linked by `sibling`. */
typedef struct {
    uint32_t a, b;     /* body indices */
    tmb_vec2 normal;   /* world, pointing from a to b */
    tmb_vec2 point;    /* world contact point */
    float penetration;
    float restitution;
    float friction;
    /* solver scratch */
    float normal_mass;
    float tangent_mass;
    float bias;
    float normal_impulse;
    float tangent_impulse;
    uint32_t sibling;  /* the other contact of this manifold, or TMB_NO_SIBLING */
} tmb_contact;

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_CONTACT_H */
