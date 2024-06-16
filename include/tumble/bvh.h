#ifndef TUMBLE_BVH_H
#define TUMBLE_BVH_H

#include "tumble/world.h"
#include "tumble/tree.h"
#include "tumble/sap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A broadphase backed by the dynamic AABB tree. Each body gets a proxy; moving
 * bodies update their proxy, and overlap pairs are gathered by querying every
 * leaf against the tree. This scales better than sweep-and-prune on clustered
 * scenes. Pairs are emitted into a tmb_pair list (shared with the SAP API). */

typedef struct {
    tmb_tree tree;
    uint32_t *proxies; /* body index -> tree proxy */
    size_t proxy_count, proxy_cap;
    tmb_pair *pairs;
    size_t pair_count, pair_cap;
} tmb_bvh;

void tmb_bvh_init(tmb_bvh *b);
void tmb_bvh_free(tmb_bvh *b);
/* Rebuild proxies for every body from scratch. */
int tmb_bvh_rebuild(tmb_bvh *b, tmb_world *w);
/* Update existing proxies for moved bodies (call after a step). */
int tmb_bvh_refresh(tmb_bvh *b, tmb_world *w);
/* Collect overlapping body pairs; returns the count or (size_t)-1 on error. */
size_t tmb_bvh_pairs(tmb_bvh *b, tmb_world *w);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_BVH_H */
