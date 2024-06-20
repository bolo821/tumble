#ifndef TUMBLE_SAP_H
#define TUMBLE_SAP_H

#include "tumble/world.h"
#include "tumble/aabb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A sweep-and-prune broadphase. Body AABB endpoints are projected onto the x
 * axis and sorted; a pair is a candidate only where their x intervals overlap,
 * after which the y interval is checked. This is cheaper than the n^2 sweep on
 * scenes that are wide and sparse. */

typedef struct {
    uint32_t a, b;
} tmb_pair;

typedef struct {
    float value;
    uint32_t body;
    int is_min;
} tmb_endpoint;

typedef struct {
    tmb_endpoint *endpoints; /* 2*N, sorted by value */
    size_t ep_count, ep_cap;
    tmb_pair *pairs;
    size_t pair_count, pair_cap;
} tmb_sap;

void tmb_sap_init(tmb_sap *s);
void tmb_sap_free(tmb_sap *s);

/* Rebuild candidate pairs from the world's current body AABBs. Returns the
 * number of pairs found, or (size_t)-1 on allocation failure. */
size_t tmb_sap_run(tmb_sap *s, tmb_world *w);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_SAP_H */
