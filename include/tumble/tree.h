#ifndef TUMBLE_TREE_H
#define TUMBLE_TREE_H

#include "tumble/aabb.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A dynamic AABB tree (bounding-volume hierarchy) for broadphase and spatial
 * queries. Leaves store a fat AABB and a user id; internal nodes bound their
 * two children. Proxies can be inserted, moved and removed in roughly
 * logarithmic time. */
#define TMB_TREE_NULL 0xffffffffu

typedef struct {
    tmb_aabb box;
    uint32_t child1, child2;
    uint32_t parent;
    int32_t height; /* -1 for a free slot */
    uint32_t id;    /* user data on leaves */
    int leaf;
} tmb_tree_node;

typedef struct {
    tmb_tree_node *nodes;
    size_t count, cap;
    uint32_t root;
    uint32_t free_list;
    float fatten;
} tmb_tree;

void tmb_tree_init(tmb_tree *t);
void tmb_tree_free(tmb_tree *t);

/* Insert a proxy bounding `box` with user id; returns the proxy (node) index. */
uint32_t tmb_tree_insert(tmb_tree *t, tmb_aabb box, uint32_t id);
void tmb_tree_remove(tmb_tree *t, uint32_t proxy);
/* Move a proxy; returns 1 if the fat AABB had to be rebuilt. */
int tmb_tree_move(tmb_tree *t, uint32_t proxy, tmb_aabb box);

/* Report the ids of every proxy whose fat AABB overlaps `box`. */
size_t tmb_tree_query(const tmb_tree *t, tmb_aabb box, uint32_t *out, size_t cap);
int tmb_tree_height(const tmb_tree *t);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_TREE_H */
