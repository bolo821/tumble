#include "tumble/bvh.h"

#include <stdlib.h>

void tmb_bvh_init(tmb_bvh *b)
{
    tmb_tree_init(&b->tree);
    b->proxies = NULL;
    b->proxy_count = b->proxy_cap = 0;
    b->pairs = NULL;
    b->pair_count = b->pair_cap = 0;
}

void tmb_bvh_free(tmb_bvh *b)
{
    tmb_tree_free(&b->tree);
    free(b->proxies);
    free(b->pairs);
    tmb_bvh_init(b);
}

static int reserve_proxies(tmb_bvh *b, size_t n)
{
    if (n <= b->proxy_cap)
        return 1;
    size_t cap = b->proxy_cap ? b->proxy_cap : 16;
    while (cap < n)
        cap *= 2;
    uint32_t *p = (uint32_t *)realloc(b->proxies, cap * sizeof(*p));
    if (!p)
        return 0;
    b->proxies = p;
    b->proxy_cap = cap;
    return 1;
}

static tmb_aabb body_box(tmb_world *w, uint32_t i)
{
    tmb_body *body = tmb_world_body(w, i);
    tmb_body_sync(body);
    return tmb_shape_aabb(&body->shape, body->xf);
}

int tmb_bvh_rebuild(tmb_bvh *b, tmb_world *w)
{
    tmb_tree_free(&b->tree);
    tmb_tree_init(&b->tree);
    size_t n = tmb_world_body_count(w);
    if (!reserve_proxies(b, n))
        return -1;
    b->proxy_count = n;
    for (size_t i = 0; i < n; i++) {
        b->proxies[i] =
            tmb_tree_insert(&b->tree, body_box(w, (uint32_t)i), (uint32_t)i);
        if (b->proxies[i] == TMB_TREE_NULL)
            return -1;
    }
    return 0;
}

int tmb_bvh_refresh(tmb_bvh *b, tmb_world *w)
{
    size_t n = tmb_world_body_count(w);
    if (n != b->proxy_count)
        return tmb_bvh_rebuild(b, w);
    for (size_t i = 0; i < n; i++)
        tmb_tree_move(&b->tree, b->proxies[i], body_box(w, (uint32_t)i));
    return 0;
}

static int push_pair(tmb_bvh *b, uint32_t i, uint32_t j)
{
    if (b->pair_count == b->pair_cap) {
        size_t cap = b->pair_cap ? b->pair_cap * 2 : 64;
        tmb_pair *p = (tmb_pair *)realloc(b->pairs, cap * sizeof(*p));
        if (!p)
            return 0;
        b->pairs = p;
        b->pair_cap = cap;
    }
    b->pairs[b->pair_count].a = i < j ? i : j;
    b->pairs[b->pair_count].b = i < j ? j : i;
    b->pair_count++;
    return 1;
}

size_t tmb_bvh_pairs(tmb_bvh *b, tmb_world *w)
{
    b->pair_count = 0;
    size_t n = tmb_world_body_count(w);
    uint32_t hits[512];
    for (size_t i = 0; i < n; i++) {
        tmb_aabb box = body_box(w, (uint32_t)i);
        size_t m = tmb_tree_query(&b->tree, box, hits, 512);
        if (m > 512)
            m = 512;
        for (size_t k = 0; k < m; k++) {
            /* emit each unordered pair once */
            if (hits[k] <= (uint32_t)i)
                continue;
            if (!push_pair(b, (uint32_t)i, hits[k]))
                return (size_t)-1;
        }
    }
    return b->pair_count;
}
