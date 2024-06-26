#include "tumble/quadtree.h"

#include <stdlib.h>

void tmb_quadtree_init(tmb_quadtree *q, tmb_aabb bounds, int max_depth)
{
    q->root = (tmb_qt_node *)calloc(1, sizeof(tmb_qt_node));
    if (q->root)
        q->root->bounds = bounds;
    q->max_depth = max_depth > 0 ? max_depth : 8;
    q->size = 0;
}

static void free_node(tmb_qt_node *n)
{
    if (!n)
        return;
    if (n->divided)
        for (int i = 0; i < 4; i++)
            free_node(n->kids[i]);
    free(n);
}

void tmb_quadtree_free(tmb_quadtree *q)
{
    free_node(q->root);
    q->root = NULL;
    q->size = 0;
}

static int subdivide(tmb_qt_node *n)
{
    tmb_vec2 c = tmb_aabb_center(n->bounds);
    tmb_aabb q0 = {{n->bounds.min.x, c.y}, {c.x, n->bounds.max.y}};
    tmb_aabb q1 = {{c.x, c.y}, {n->bounds.max.x, n->bounds.max.y}};
    tmb_aabb q2 = {{n->bounds.min.x, n->bounds.min.y}, {c.x, c.y}};
    tmb_aabb q3 = {{c.x, n->bounds.min.y}, {n->bounds.max.x, c.y}};
    tmb_aabb boxes[4] = {q0, q1, q2, q3};
    for (int i = 0; i < 4; i++) {
        n->kids[i] = (tmb_qt_node *)calloc(1, sizeof(tmb_qt_node));
        if (!n->kids[i])
            return -1;
        n->kids[i]->bounds = boxes[i];
    }
    n->divided = 1;
    return 0;
}

static int insert_node(tmb_qt_node *n, tmb_vec2 p, uint32_t id, int depth,
                       int max_depth)
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
    if (!n->divided && subdivide(n) != 0)
        return 0;
    for (int i = 0; i < 4; i++)
        if (insert_node(n->kids[i], p, id, depth + 1, max_depth))
            return 1;
    return 0;
}

int tmb_quadtree_insert(tmb_quadtree *q, tmb_vec2 p, uint32_t id)
{
    if (!q->root)
        return 0;
    int ok = insert_node(q->root, p, id, 0, q->max_depth);
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
