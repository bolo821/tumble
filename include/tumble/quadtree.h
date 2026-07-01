#ifndef TUMBLE_QUADTREE_H
#define TUMBLE_QUADTREE_H

#include "tumble/aabb.h"
#include "tumble/pool.h"
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A point-region quadtree over a fixed world bound. Items (a position plus a
 * user id) are inserted and a node subdivides once it holds more than a
 * threshold. Region queries return the ids inside an AABB. A complementary
 * broadphase to the AABB tree, better when items are points (particles, spawn
 * markers) rather than extended shapes. */

#define TMB_QT_NODE_CAP 8

typedef struct {
    tmb_vec2 pos;
    uint32_t id;
} tmb_qt_item;

typedef struct tmb_qt_node tmb_qt_node;
struct tmb_qt_node {
    tmb_aabb bounds;
    tmb_qt_item items[TMB_QT_NODE_CAP];
    int count;
    int divided;
    tmb_qt_node *kids[4];
};

typedef struct {
    tmb_qt_node *root;
    int max_depth;
    size_t size;
    tmb_pool nodes; /* owns every node's memory (root and children alike) */
} tmb_quadtree;

void tmb_quadtree_init(tmb_quadtree *q, tmb_aabb bounds, int max_depth);
void tmb_quadtree_free(tmb_quadtree *q);
/* Empty the tree for reuse across frames without freeing the root node. */
void tmb_quadtree_clear(tmb_quadtree *q);
int tmb_quadtree_insert(tmb_quadtree *q, tmb_vec2 p, uint32_t id);
size_t tmb_quadtree_query(const tmb_quadtree *q, tmb_aabb region, uint32_t *out,
                          size_t cap);
int tmb_quadtree_depth(const tmb_quadtree *q);

#ifdef __cplusplus
}
#endif

#endif /* TUMBLE_QUADTREE_H */
