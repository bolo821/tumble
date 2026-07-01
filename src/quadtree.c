#include "tumble/quadtree.h"

#include <stdlib.h>

/* Every node -- the root and all children -- is carved from q->nodes, a
 * fixed-size block pool. This keeps node lifetime under a single owner: the
 * pool. Releasing a node returns its block to the pool's free list; freeing
 * the pool reclaims everything at once. Because the pool owns the memory, the
 * tree must never keep a pointer to a released node -- clear() therefore both
 * releases the child subtrees AND clears the parent's divided flag and child
 * pointers, so no traversal can follow a dangling link. */

static tmb_qt_node *node_new(tmb_quadtree *q, tmb_aabb bounds)
{
    tmb_qt_node *n = (tmb_qt_node *)tmb_pool_calloc(&q->nodes);
    if (n)
        n->bounds = bounds;
    return n;
}

/* Recursively release a subtree back to the pool (post-order). */
static void release_subtree(tmb_quadtree *q, tmb_qt_node *n)
{
    if (!n)
        return;
    if (n->divided)
        for (int i = 0; i < 4; i++) {
            release_subtree(q, n->kids[i]);
            n->kids[i] = NULL;
        }
    n->divided = 0;
    tmb_pool_release(&q->nodes, n);
}

/* Drop a node's children (if any) and turn it back into a leaf, releasing the
 * child blocks to the pool. Leaves no dangling pointers behind. */
static void make_leaf(tmb_quadtree *q, tmb_qt_node *n)
{
    if (!n || !n->divided)
        return;
    for (int i = 0; i < 4; i++) {
        release_subtree(q, n->kids[i]);
        n->kids[i] = NULL;
    }
    n->divided = 0;
}

void tmb_quadtree_init(tmb_quadtree *q, tmb_aabb bounds, int max_depth)
{
    tmb_pool_init(&q->nodes, sizeof(tmb_qt_node), 64);
    q->root = node_new(q, bounds);
    q->max_depth = max_depth > 0 ? max_depth : 8;
    q->size = 0;
}

void tmb_quadtree_free(tmb_quadtree *q)
{
    /* The pool owns all node memory, so a single pool_free reclaims the whole
     * tree; there is no per-node walk that could double-free. */
    tmb_pool_free(&q->nodes);
    q->root = NULL;
    q->size = 0;
}

/* Empty the tree for reuse across frames without giving up the root node. The
 * children accumulated last frame are released back to the pool and the root
 * is turned back into an empty leaf, so no stale child pointer survives. */
void tmb_quadtree_clear(tmb_quadtree *q)
{
    if (!q || !q->root)
        return;
    make_leaf(q, q->root);
    q->root->count = 0;
    q->size = 0;
}

static int subdivide(tmb_quadtree *q, tmb_qt_node *n)
{
    tmb_vec2 c = tmb_aabb_center(n->bounds);
    tmb_aabb q0 = {{n->bounds.min.x, c.y}, {c.x, n->bounds.max.y}};
    tmb_aabb q1 = {{c.x, c.y}, {n->bounds.max.x, n->bounds.max.y}};
    tmb_aabb q2 = {{n->bounds.min.x, n->bounds.min.y}, {c.x, c.y}};
    tmb_aabb q3 = {{c.x, n->bounds.min.y}, {n->bounds.max.x, c.y}};
    tmb_aabb boxes[4] = {q0, q1, q2, q3};
    for (int i = 0; i < 4; i++) {
        n->kids[i] = node_new(q, boxes[i]);
        if (!n->kids[i]) {
            /* roll back any children created so far to avoid a partial split */
            for (int j = 0; j < i; j++) {
                tmb_pool_release(&q->nodes, n->kids[j]);
                n->kids[j] = NULL;
            }
            return -1;
        }
    }
    n->divided = 1;
    return 0;
}

static int insert_node(tmb_quadtree *q, tmb_qt_node *n, tmb_vec2 p, uint32_t id,
                       int depth, int max_depth)
{
    if (!tmb_aabb_contains(n->bounds, p))
        return 0;
    if (n->count < TMB_QT_NODE_CAP || depth >= max_depth) {
        if (n->count < TMB_QT_NODE_CAP) {
            n->items[n->count].pos = p;
            n->items[n->count].id = id;
            n->count++;
            return 1;
        }
        /* at max depth: overwrite policy is to drop; report not stored */
        return 0;
    }
    if (!n->divided && subdivide(q, n) != 0)
        return 0;
    for (int i = 0; i < 4; i++)
        if (insert_node(q, n->kids[i], p, id, depth + 1, max_depth))
            return 1;
    return 0;
}

int tmb_quadtree_insert(tmb_quadtree *q, tmb_vec2 p, uint32_t id)
{
    if (!q->root)
        return 0;
    int ok = insert_node(q, q->root, p, id, 0, q->max_depth);
    if (ok)
        q->size++;
    return ok;
}

static void query_node(const tmb_qt_node *n, tmb_aabb region, uint32_t *out,
                       size_t cap, size_t *count)
{
    if (!n || !tmb_aabb_overlap(n->bounds, region))
        return;
    for (int i = 0; i < n->count; i++) {
        if (tmb_aabb_contains(region, n->items[i].pos)) {
            if (*count < cap)
                out[*count] = n->items[i].id;
            (*count)++;
        }
    }
    if (n->divided)
        for (int i = 0; i < 4; i++)
            query_node(n->kids[i], region, out, cap, count);
}

size_t tmb_quadtree_query(const tmb_quadtree *q, tmb_aabb region, uint32_t *out,
                          size_t cap)
{
    size_t count = 0;
    query_node(q->root, region, out, cap, &count);
    return count;
}

static int depth_of(const tmb_qt_node *n)
{
    if (!n || !n->divided)
        return 1;
    int m = 0;
    for (int i = 0; i < 4; i++) {
        int d = depth_of(n->kids[i]);
        if (d > m)
            m = d;
    }
    return m + 1;
}

int tmb_quadtree_depth(const tmb_quadtree *q)
{
    return depth_of(q->root);
}
